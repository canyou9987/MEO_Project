// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Components/PassiveTreeComponent.h"
#include "MEO_Project/Components/StatusComponent.h"
#include "Containers/Queue.h"

// Sets default values for this component's properties
UPassiveTreeComponent::UPassiveTreeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UPassiveTreeComponent::TryAllocateNode(FName NodeID)
{
    FPassiveGraphNode* TargetNode = NodeGraph.Find(NodeID);
    if (!TargetNode || TargetNode->bIsAllocated) return false;

    // 인접 노드 중 활성화된 노드가 최소 1개 이상 연결되어 있는지 검증
    bool bHasActiveNeighbor = (NodeID == ClassStartNodeID);
    if (!bHasActiveNeighbor)
    {
        for (const FName& NeighborID : TargetNode->NeighborNodeIDs)
        {
            if (AllocatedNodeIDs.Contains(NeighborID))
            {
                bHasActiveNeighbor = true;
                break;
            }
        }
    }

    if (!bHasActiveNeighbor) return false;

    TargetNode->bIsAllocated = true;
    AllocatedNodeIDs.Add(NodeID);

    ReapplyTreeModifiers();
    return true;
}

bool UPassiveTreeComponent::TryRefundNode(FName NodeID)
{
    if (!CanRefundNode(NodeID)) return false;

    FPassiveGraphNode* TargetNode = NodeGraph.Find(NodeID);
    if (!TargetNode) return false;

    TargetNode->bIsAllocated = false;
    AllocatedNodeIDs.Remove(NodeID);

    ReapplyTreeModifiers();
    return true;
}

bool UPassiveTreeComponent::CanRefundNode(FName NodeIDToRefund) const
{
    // 시작 노드이거나 이미 활성화되어 있지 않으면 반환 불가
    if (!AllocatedNodeIDs.Contains(NodeIDToRefund) || NodeIDToRefund == ClassStartNodeID)
    {
        return false;
    }

    // 반환 후 목표 도달 활성 노드 개수
    const int32 TargetVisitedCount = AllocatedNodeIDs.Num() - 1;
    if (TargetVisitedCount <= 0) return true;

    // BFS 탐색 준비 (NodeIDToRefund 노드를 경로에서 차단)
    TSet<FName> VisitedNodes;
    TQueue<FName> SearchQueue;

    SearchQueue.Enqueue(ClassStartNodeID);
    VisitedNodes.Add(ClassStartNodeID);

    while (!SearchQueue.IsEmpty())
    {
        FName CurrentNodeID;
        SearchQueue.Dequeue(CurrentNodeID);

        const FPassiveGraphNode* CurrentNode = NodeGraph.Find(CurrentNodeID);
        if (!CurrentNode) continue;

        for (const FName& NeighborID : CurrentNode->NeighborNodeIDs)
        {
            // 반환하려는 대상 노드는 탐색 경로에서 완전 제외 (Blocked)
            if (NeighborID == NodeIDToRefund) continue;

            if (AllocatedNodeIDs.Contains(NeighborID) && !VisitedNodes.Contains(NeighborID))
            {
                VisitedNodes.Add(NeighborID);
                SearchQueue.Enqueue(NeighborID);
            }
        }
    }

    // BFS 탐색으로 방문한 활성 노드 수가 남은 전체 노드 수와 일치해야 섬 고립(Isolated Island)이 발생하지 않음
    return VisitedNodes.Num() == TargetVisitedCount;
}

void UPassiveTreeComponent::OnWeaponSetSwapped(int32 NewWeaponSetIndex)
{
    if (CurrentWeaponSetIndex == NewWeaponSetIndex) return;

    CurrentWeaponSetIndex = NewWeaponSetIndex;
    ReapplyTreeModifiers();
}

void UPassiveTreeComponent::ReapplyTreeModifiers()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return;

    UStatusComponent* StatusComp = OwnerActor->FindComponentByClass<UStatusComponent>();
    if (!StatusComp) return;

    // StatusComponent 스탯 일괄 재계산
    StatusComp->RecalculateStats();

    // 현재 활성화된 패시브 노드 스탯 모디파이어 주입 (Dual Spec 필터링)
    for (const FName& NodeID : AllocatedNodeIDs)
    {
        const FPassiveGraphNode* Node = NodeGraph.Find(NodeID);
        if (!Node) continue;

        // Common 노드이거나 현재 주 무기 세트 번호와 부합하는 노드만 스탯 적용
        bool bEligibleForWeaponSet = (Node->WeaponSetBinding == EWeaponSetBinding::Common) ||
            (CurrentWeaponSetIndex == 0 && Node->WeaponSetBinding == EWeaponSetBinding::WeaponSet1) ||
            (CurrentWeaponSetIndex == 1 && Node->WeaponSetBinding == EWeaponSetBinding::WeaponSet2);

        if (bEligibleForWeaponSet)
        {
            for (const FStatModifierData& Mod : Node->Modifiers)
            {
                StatusComp->ApplyStatModifier(Mod);
            }
        }
    }
}
