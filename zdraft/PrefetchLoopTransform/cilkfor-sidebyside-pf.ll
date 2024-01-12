; ModuleID = './cilkfor-sidebyside-pf.bc'
source_filename = "cilkfor-sidebyside.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4loopPiS_i(i32* noundef %A, i32* noundef %B, i32 noundef %x) #0 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  %cmp = icmp sgt i32 %x, 100
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br i1 true, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %if.then
  %0 = mul i32 1, 2
  %1 = mul i32 3, %0
  %2 = sub i32 %x, %1
  br label %for.body.lr.ph.split.pl.precheck

for.body.lr.ph.split.pl.precheck:                 ; preds = %for.body.lr.ph
  %3 = icmp slt i32 0, %2
  br i1 %3, label %for.body.lr.ph.split.pl.ph, label %for.body.lr.ph.split

for.body.lr.ph.split.pl.ph:                       ; preds = %for.body.lr.ph.split.pl.precheck
  br label %for.body.lr.ph.split.pl.body

for.body.lr.ph.split.pl.body:                     ; preds = %for.body.lr.ph.split.pl.body, %for.body.lr.ph.split.pl.ph
  %pl.idv = phi i32 [ 0, %for.body.lr.ph.split.pl.ph ], [ %4, %for.body.lr.ph.split.pl.body ]
  %4 = add i32 %pl.idv, 1
  %5 = icmp slt i32 %4, %2
  br i1 %5, label %for.body.lr.ph.split.pl.body, label %for.body.lr.ph.split.loopexit

for.body.lr.ph.split.loopexit:                    ; preds = %for.body.lr.ph.split.pl.body
  br label %for.body.lr.ph.split

for.body.lr.ph.split:                             ; preds = %for.body.lr.ph.split.loopexit, %for.body.lr.ph.split.pl.precheck
  br label %for.body.peel.begin

for.body.peel.begin:                              ; preds = %for.body.lr.ph.split
  br label %for.body.peel

for.body.peel:                                    ; preds = %for.body.peel.begin
  %add.peel = add nuw nsw i32 0, 1
  %idxprom.peel = zext i32 %add.peel to i64
  %arrayidx.peel = getelementptr inbounds i32, i32* %B, i64 %idxprom.peel
  %6 = load i32, i32* %arrayidx.peel, align 4
  %idxprom2.peel = zext i32 0 to i64
  %arrayidx3.peel = getelementptr inbounds i32, i32* %B, i64 %idxprom2.peel
  store i32 %6, i32* %arrayidx3.peel, align 4
  br label %for.inc.peel

for.inc.peel:                                     ; preds = %for.body.peel
  %inc.peel = add nuw nsw i32 0, 1
  %cmp1.peel = icmp slt i32 %inc.peel, %2
  br i1 %cmp1.peel, label %for.body.peel.next, label %for.cond.for.end_crit_edge, !llvm.loop !4

for.body.peel.next:                               ; preds = %for.inc.peel
  br label %for.body.lr.ph.split.peel.newph

for.body.lr.ph.split.peel.newph:                  ; preds = %for.body.peel.next
  br label %for.body

for.body:                                         ; preds = %for.inc, %for.body.lr.ph.split.peel.newph
  %j.02 = phi i32 [ %inc.peel, %for.body.lr.ph.split.peel.newph ], [ %inc, %for.inc ]
  %add = add nuw nsw i32 %j.02, 1
  %idxprom = zext i32 %add to i64
  %arrayidx = getelementptr inbounds i32, i32* %B, i64 %idxprom
  %7 = load i32, i32* %arrayidx, align 4
  %idxprom2 = zext i32 %j.02 to i64
  %arrayidx3 = getelementptr inbounds i32, i32* %B, i64 %idxprom2
  store i32 %7, i32* %arrayidx3, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %j.02, 1
  %cmp1 = icmp slt i32 %inc, %2
  br i1 %cmp1, label %for.body, label %for.cond.for.end_crit_edge.loopexit, !llvm.loop !4

for.cond.for.end_crit_edge.loopexit:              ; preds = %for.inc
  br label %for.cond.for.end_crit_edge

for.cond.for.end_crit_edge:                       ; preds = %for.cond.for.end_crit_edge.loopexit, %for.inc.peel
  br label %for.body.lr.ph1

for.body.lr.ph1:                                  ; preds = %for.cond.for.end_crit_edge
  %8 = mul i32 1, 2
  %9 = mul i32 3, %8
  %10 = sub i32 %x, %9
  br label %for.body2

for.body2:                                        ; preds = %for.inc9, %for.body.lr.ph1
  %j.023 = phi i32 [ %10, %for.body.lr.ph1 ], [ %inc12, %for.inc9 ]
  %add4 = add nuw nsw i32 %j.023, 1
  %idxprom5 = zext i32 %add4 to i64
  %arrayidx6 = getelementptr inbounds i32, i32* %B, i64 %idxprom5
  %11 = load i32, i32* %arrayidx6, align 4
  %idxprom27 = zext i32 %j.023 to i64
  %arrayidx38 = getelementptr inbounds i32, i32* %B, i64 %idxprom27
  store i32 %11, i32* %arrayidx38, align 4
  br label %for.inc9

for.inc9:                                         ; preds = %for.body2
  %inc12 = add nuw nsw i32 %j.023, 1
  %cmp113 = icmp slt i32 %inc12, %x
  br i1 %cmp113, label %for.body2, label %for.cond.for.end_crit_edge.split, !llvm.loop !4

for.cond.for.end_crit_edge.split:                 ; preds = %for.inc9
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge.split, %if.then
  br label %if.end

if.else:                                          ; preds = %entry
  %cmp4 = icmp sgt i32 %x, 0
  br i1 %cmp4, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %if.else
  %12 = mul i32 1, 2
  %13 = mul i32 3, %12
  %14 = sub i32 %x, %13
  br label %pfor.ph.split.pl.precheck

pfor.ph.split.pl.precheck:                        ; preds = %pfor.ph
  %15 = icmp slt i32 0, %14
  br i1 %15, label %pfor.ph.split.pl.ph, label %pfor.ph.split

pfor.ph.split.pl.ph:                              ; preds = %pfor.ph.split.pl.precheck
  br label %pfor.ph.split.pl.body

pfor.ph.split.pl.body:                            ; preds = %pfor.ph.split.pl.body, %pfor.ph.split.pl.ph
  %pl.idv27 = phi i32 [ 0, %pfor.ph.split.pl.ph ], [ %16, %pfor.ph.split.pl.body ]
  %16 = add i32 %pl.idv27, 1
  %17 = icmp slt i32 %16, %14
  br i1 %17, label %pfor.ph.split.pl.body, label %pfor.ph.split.loopexit

pfor.ph.split.loopexit:                           ; preds = %pfor.ph.split.pl.body
  br label %pfor.ph.split

pfor.ph.split:                                    ; preds = %pfor.ph.split.loopexit, %pfor.ph.split.pl.precheck
  br label %pfor.cond.peel.begin

pfor.cond.peel.begin:                             ; preds = %pfor.ph.split
  br label %pfor.cond.peel

pfor.cond.peel:                                   ; preds = %pfor.cond.peel.begin
  br label %pfor.detach.peel

pfor.detach.peel:                                 ; preds = %pfor.cond.peel
  detach within %syncreg, label %pfor.body.entry.peel, label %pfor.inc.peel

pfor.body.entry.peel:                             ; preds = %pfor.detach.peel
  br label %pfor.body.peel

pfor.body.peel:                                   ; preds = %pfor.body.entry.peel
  %idxprom8.peel = zext i32 0 to i64
  %arrayidx9.peel = getelementptr inbounds i32, i32* %A, i64 %idxprom8.peel
  %18 = load i32, i32* %arrayidx9.peel, align 4
  %inc10.peel = add nsw i32 %18, 1
  store i32 %inc10.peel, i32* %arrayidx9.peel, align 4
  br label %pfor.preattach.peel

pfor.preattach.peel:                              ; preds = %pfor.body.peel
  reattach within %syncreg, label %pfor.inc.peel

pfor.inc.peel:                                    ; preds = %pfor.preattach.peel, %pfor.detach.peel
  %inc11.peel = add nuw nsw i32 0, 1
  %cmp12.peel = icmp slt i32 %inc11.peel, %14
  br i1 %cmp12.peel, label %pfor.cond.peel.next, label %pfor.cond.cleanup, !llvm.loop !6

pfor.cond.peel.next:                              ; preds = %pfor.inc.peel
  br label %pfor.ph.split.peel.newph

pfor.ph.split.peel.newph:                         ; preds = %pfor.cond.peel.next
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph.split.peel.newph
  %__begin.0 = phi i32 [ %inc11.peel, %pfor.ph.split.peel.newph ], [ %inc11, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %idxprom8 = zext i32 %__begin.0 to i64
  %arrayidx9 = getelementptr inbounds i32, i32* %A, i64 %idxprom8
  %19 = load i32, i32* %arrayidx9, align 4
  %inc10 = add nsw i32 %19, 1
  store i32 %inc10, i32* %arrayidx9, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc11 = add nuw nsw i32 %__begin.0, 1
  %cmp12 = icmp slt i32 %inc11, %14
  br i1 %cmp12, label %pfor.cond, label %pfor.cond.cleanup.loopexit, !llvm.loop !6

pfor.cond.cleanup.loopexit:                       ; preds = %pfor.inc
  br label %pfor.cond.cleanup

pfor.cond.cleanup:                                ; preds = %pfor.cond.cleanup.loopexit, %pfor.inc.peel
  br label %pfor.ph14

pfor.ph14:                                        ; preds = %pfor.cond.cleanup
  %20 = mul i32 1, 2
  %21 = mul i32 3, %20
  %22 = sub i32 %x, %21
  br label %pfor.cond15

pfor.cond15:                                      ; preds = %pfor.inc24, %pfor.ph14
  %__begin.016 = phi i32 [ %22, %pfor.ph14 ], [ %inc1125, %pfor.inc24 ]
  br label %pfor.detach17

pfor.detach17:                                    ; preds = %pfor.cond15
  detach within %syncreg, label %pfor.body.entry18, label %pfor.inc24

pfor.body.entry18:                                ; preds = %pfor.detach17
  br label %pfor.body19

pfor.body19:                                      ; preds = %pfor.body.entry18
  %idxprom820 = zext i32 %__begin.016 to i64
  %arrayidx921 = getelementptr inbounds i32, i32* %A, i64 %idxprom820
  %23 = load i32, i32* %arrayidx921, align 4
  %inc1022 = add nsw i32 %23, 1
  store i32 %inc1022, i32* %arrayidx921, align 4
  br label %pfor.preattach23

pfor.preattach23:                                 ; preds = %pfor.body19
  reattach within %syncreg, label %pfor.inc24

pfor.inc24:                                       ; preds = %pfor.preattach23, %pfor.detach17
  %inc1125 = add nuw nsw i32 %__begin.016, 1
  %cmp1226 = icmp slt i32 %inc1125, %x
  br i1 %cmp1226, label %pfor.cond15, label %pfor.cond.cleanup.split, !llvm.loop !6

pfor.cond.cleanup.split:                          ; preds = %pfor.inc24
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup.split
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %if.else
  br label %if.end

if.end:                                           ; preds = %pfor.end, %for.end
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
