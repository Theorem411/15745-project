; ModuleID = './toy-peel.bc'
source_filename = "cilkfor-sidebyside.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4loopPiS_i(i32* noundef %A, i32* noundef %B, i32 noundef %x) #0 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  %cmp = icmp sgt i32 %x, 100
  br i1 %cmp, label %for.cond.preheader, label %if.else

for.cond.preheader:                               ; preds = %entry
  br label %for.body.peel.begin

for.body.peel.begin:                              ; preds = %for.cond.preheader
  br label %for.body.peel

for.body.peel:                                    ; preds = %for.body.peel.begin
  %add.peel = add nuw nsw i32 0, 1
  %idxprom.peel = zext i32 %add.peel to i64
  %arrayidx.peel = getelementptr inbounds i32, i32* %B, i64 %idxprom.peel
  %0 = load i32, i32* %arrayidx.peel, align 4
  %idxprom2.peel = zext i32 0 to i64
  %arrayidx3.peel = getelementptr inbounds i32, i32* %B, i64 %idxprom2.peel
  store i32 %0, i32* %arrayidx3.peel, align 4
  %inc.peel = add nuw nsw i32 0, 1
  %cmp1.peel = icmp ult i32 0, 9
  br i1 %cmp1.peel, label %for.body.peel.next, label %if.end.loopexit, !llvm.loop !4

for.body.peel.next:                               ; preds = %for.body.peel
  br label %for.cond.preheader.peel.newph

for.cond.preheader.peel.newph:                    ; preds = %for.body.peel.next
  br label %for.body

for.body:                                         ; preds = %for.body, %for.cond.preheader.peel.newph
  %j.01 = phi i32 [ %inc.peel, %for.cond.preheader.peel.newph ], [ %inc, %for.body ]
  %add = add nuw nsw i32 %j.01, 1
  %idxprom = zext i32 %add to i64
  %arrayidx = getelementptr inbounds i32, i32* %B, i64 %idxprom
  %1 = load i32, i32* %arrayidx, align 4
  %idxprom2 = zext i32 %j.01 to i64
  %arrayidx3 = getelementptr inbounds i32, i32* %B, i64 %idxprom2
  store i32 %1, i32* %arrayidx3, align 4
  %inc = add nuw nsw i32 %j.01, 1
  %cmp1 = icmp ult i32 %j.01, 9
  br i1 %cmp1, label %for.body, label %if.end.loopexit.loopexit, !llvm.loop !4

if.else:                                          ; preds = %entry
  br i1 true, label %pfor.ph, label %if.end

pfor.ph:                                          ; preds = %if.else
  br label %pfor.cond.peel.begin

pfor.cond.peel.begin:                             ; preds = %pfor.ph
  br label %pfor.cond.peel

pfor.cond.peel:                                   ; preds = %pfor.cond.peel.begin
  detach within %syncreg, label %pfor.body.peel, label %pfor.inc.peel

pfor.body.peel:                                   ; preds = %pfor.cond.peel
  %idxprom8.peel = zext i32 0 to i64
  %arrayidx9.peel = getelementptr inbounds i32, i32* %A, i64 %idxprom8.peel
  %2 = load i32, i32* %arrayidx9.peel, align 4
  %inc10.peel = add nsw i32 %2, 1
  store i32 %inc10.peel, i32* %arrayidx9.peel, align 4
  reattach within %syncreg, label %pfor.inc.peel

pfor.inc.peel:                                    ; preds = %pfor.body.peel, %pfor.cond.peel
  %inc11.peel = add nuw nsw i32 0, 1
  %cmp12.peel = icmp ult i32 0, 9
  br i1 %cmp12.peel, label %pfor.cond.peel.next, label %pfor.cond.cleanup, !llvm.loop !6

pfor.cond.peel.next:                              ; preds = %pfor.inc.peel
  br label %pfor.ph.peel.newph

pfor.ph.peel.newph:                               ; preds = %pfor.cond.peel.next
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph.peel.newph
  %__begin.0 = phi i32 [ %inc11.peel, %pfor.ph.peel.newph ], [ %inc11, %pfor.inc ]
  detach within %syncreg, label %pfor.body, label %pfor.inc

pfor.body:                                        ; preds = %pfor.cond
  %idxprom8 = zext i32 %__begin.0 to i64
  %arrayidx9 = getelementptr inbounds i32, i32* %A, i64 %idxprom8
  %3 = load i32, i32* %arrayidx9, align 4
  %inc10 = add nsw i32 %3, 1
  store i32 %inc10, i32* %arrayidx9, align 4
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.body, %pfor.cond
  %inc11 = add nuw nsw i32 %__begin.0, 1
  %cmp12 = icmp ult i32 %__begin.0, 9
  br i1 %cmp12, label %pfor.cond, label %pfor.cond.cleanup.loopexit, !llvm.loop !6

pfor.cond.cleanup.loopexit:                       ; preds = %pfor.inc
  br label %pfor.cond.cleanup

pfor.cond.cleanup:                                ; preds = %pfor.cond.cleanup.loopexit, %pfor.inc.peel
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %if.end

if.end.loopexit.loopexit:                         ; preds = %for.body
  br label %if.end.loopexit

if.end.loopexit:                                  ; preds = %if.end.loopexit.loopexit, %for.body.peel
  br label %if.end

if.end:                                           ; preds = %if.end.loopexit, %sync.continue, %if.else
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #1

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #2

attributes #0 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { argmemonly willreturn }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !7}
!7 = !{!"tapir.loop.spawn.strategy", i32 1}
