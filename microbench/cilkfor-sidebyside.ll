; ModuleID = 'cilkfor-sidebyside.bc'
source_filename = "cilkfor-sidebyside.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4loopPii(i32* noundef %A, i32 noundef %x) #0 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  %call = call noalias noundef nonnull dereferenceable(400) i8* @_Znam(i64 noundef 400) #4
  %0 = bitcast i8* %call to [10 x i32]*
  %cmp = icmp sgt i32 %x, 100
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br i1 true, label %for.body.lr.ph, label %for.end15

for.body.lr.ph:                                   ; preds = %if.then
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc13
  %i.04 = phi i32 [ 0, %for.body.lr.ph ], [ %inc14, %for.inc13 ]
  %cmp31 = icmp sgt i32 %x, 0
  br i1 %cmp31, label %for.body4.lr.ph, label %for.end

for.body4.lr.ph:                                  ; preds = %for.body
  br label %for.body4

for.body4:                                        ; preds = %for.body4.lr.ph, %for.inc
  %j.02 = phi i32 [ 0, %for.body4.lr.ph ], [ %inc, %for.inc ]
  %idxprom = zext i32 %i.04 to i64
  %idxprom5 = zext i32 %j.02 to i64
  %arrayidx6 = getelementptr inbounds [10 x i32], [10 x i32]* %0, i64 %idxprom, i64 %idxprom5
  %1 = load i32, i32* %arrayidx6, align 4
  %add = add nuw nsw i32 %i.04, 1
  %mul.neg = mul i32 %j.02, -2
  %sub = add i32 %mul.neg, %add
  %idxprom7 = sext i32 %sub to i64
  %sub9 = sub nsw i32 %i.04, %j.02
  %add10 = add nsw i32 %sub9, 2
  %idxprom11 = sext i32 %add10 to i64
  %arrayidx12 = getelementptr inbounds [10 x i32], [10 x i32]* %0, i64 %idxprom7, i64 %idxprom11
  store i32 %1, i32* %arrayidx12, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body4
  %inc = add nuw nsw i32 %j.02, 1
  %cmp3 = icmp slt i32 %inc, %x
  br i1 %cmp3, label %for.body4, label %for.cond2.for.end_crit_edge, !llvm.loop !4

for.cond2.for.end_crit_edge:                      ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond2.for.end_crit_edge, %for.body
  br label %for.inc13

for.inc13:                                        ; preds = %for.end
  %inc14 = add nuw nsw i32 %i.04, 1
  %cmp1 = icmp slt i32 %inc14, %x
  br i1 %cmp1, label %for.body, label %for.cond.for.end15_crit_edge, !llvm.loop !6

for.cond.for.end15_crit_edge:                     ; preds = %for.inc13
  br label %for.end15

for.end15:                                        ; preds = %for.cond.for.end15_crit_edge, %if.then
  br label %if.end

if.else:                                          ; preds = %entry
  %cmp16 = icmp sgt i32 %x, 0
  br i1 %cmp16, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %if.else
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc26, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %idxprom23 = zext i32 %__begin.0 to i64
  %arrayidx24 = getelementptr inbounds i32, i32* %A, i64 %idxprom23
  %2 = load i32, i32* %arrayidx24, align 4
  %inc25 = add nsw i32 %2, 1
  store i32 %inc25, i32* %arrayidx24, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc26 = add nuw nsw i32 %__begin.0, 1
  %cmp27 = icmp slt i32 %inc26, %x
  br i1 %cmp27, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !7

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %if.else
  br label %if.end

if.end:                                           ; preds = %pfor.end, %for.end15
  ret void
}

; Function Attrs: nobuiltin allocsize(0)
declare dso_local noundef nonnull i8* @_Znam(i64 noundef) #1

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #2

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #3

attributes #0 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nobuiltin allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { argmemonly nounwind willreturn }
attributes #3 = { argmemonly willreturn }
attributes #4 = { builtin allocsize(0) }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !5}
!7 = distinct !{!7, !8}
!8 = !{!"tapir.loop.spawn.strategy", i32 1}
