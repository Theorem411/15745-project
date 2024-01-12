; ModuleID = './for-simple-peel.bc'
source_filename = "for-simple.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4loopPA10_iS0_i([10 x i32]* noundef %A, [10 x i32]* noundef %B, i32 noundef %N) #0 {
entry:
  %cmp1 = icmp sgt i32 %N, 0
  br i1 %cmp1, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %entry
  %0 = add i32 %N, -1
  %xtraiter = and i32 %N, 1
  %1 = icmp ult i32 %0, 1
  br i1 %1, label %for.cond.for.end_crit_edge.unr-lcssa, label %for.body.lr.ph.new

for.body.lr.ph.new:                               ; preds = %for.body.lr.ph
  %unroll_iter = sub i32 %N, %xtraiter
  br label %for.body

for.body:                                         ; preds = %for.inc.1, %for.body.lr.ph.new
  %i.02 = phi i32 [ 0, %for.body.lr.ph.new ], [ %inc.1, %for.inc.1 ]
  %niter = phi i32 [ 0, %for.body.lr.ph.new ], [ %niter.next.1, %for.inc.1 ]
  %idxprom = zext i32 %i.02 to i64
  %arrayidx1 = getelementptr inbounds [10 x i32], [10 x i32]* %B, i64 0, i64 %idxprom
  %2 = load i32, i32* %arrayidx1, align 4
  %cmp2 = icmp slt i32 %2, 10
  br i1 %cmp2, label %if.then, label %if.else

if.then:                                          ; preds = %for.body
  %idxprom3 = zext i32 %i.02 to i64
  %arrayidx5 = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom3, i64 0
  store i32 1, i32* %arrayidx5, align 4
  br label %for.inc

if.else:                                          ; preds = %for.body
  %idxprom6 = zext i32 %i.02 to i64
  %arrayidx8 = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom6, i64 0
  store i32 0, i32* %arrayidx8, align 4
  br label %for.inc

for.inc:                                          ; preds = %if.else, %if.then
  %inc = add nuw nsw i32 %i.02, 1
  %niter.next = add nuw nsw i32 %niter, 1
  %idxprom.1 = zext i32 %inc to i64
  %arrayidx1.1 = getelementptr inbounds [10 x i32], [10 x i32]* %B, i64 0, i64 %idxprom.1
  %3 = load i32, i32* %arrayidx1.1, align 4
  %cmp2.1 = icmp slt i32 %3, 10
  br i1 %cmp2.1, label %if.then.1, label %if.else.1

if.else.1:                                        ; preds = %for.inc
  %idxprom6.1 = zext i32 %inc to i64
  %arrayidx8.1 = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom6.1, i64 0
  store i32 0, i32* %arrayidx8.1, align 4
  br label %for.inc.1

if.then.1:                                        ; preds = %for.inc
  %idxprom3.1 = zext i32 %inc to i64
  %arrayidx5.1 = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom3.1, i64 0
  store i32 1, i32* %arrayidx5.1, align 4
  br label %for.inc.1

for.inc.1:                                        ; preds = %if.then.1, %if.else.1
  %inc.1 = add nuw nsw i32 %inc, 1
  %niter.next.1 = add i32 %niter.next, 1
  %niter.ncmp.1 = icmp ne i32 %niter.next.1, %unroll_iter
  br i1 %niter.ncmp.1, label %for.body, label %for.cond.for.end_crit_edge.unr-lcssa.loopexit, !llvm.loop !4

for.cond.for.end_crit_edge.unr-lcssa.loopexit:    ; preds = %for.inc.1
  %i.02.unr.ph = phi i32 [ %inc.1, %for.inc.1 ]
  br label %for.cond.for.end_crit_edge.unr-lcssa

for.cond.for.end_crit_edge.unr-lcssa:             ; preds = %for.cond.for.end_crit_edge.unr-lcssa.loopexit, %for.body.lr.ph
  %i.02.unr = phi i32 [ 0, %for.body.lr.ph ], [ %i.02.unr.ph, %for.cond.for.end_crit_edge.unr-lcssa.loopexit ]
  %lcmp.mod = icmp ne i32 %xtraiter, 0
  br i1 %lcmp.mod, label %for.body.epil.preheader, label %for.cond.for.end_crit_edge

for.body.epil.preheader:                          ; preds = %for.cond.for.end_crit_edge.unr-lcssa
  br label %for.body.epil

for.body.epil:                                    ; preds = %for.body.epil.preheader
  %idxprom.epil = zext i32 %i.02.unr to i64
  %arrayidx1.epil = getelementptr inbounds [10 x i32], [10 x i32]* %B, i64 0, i64 %idxprom.epil
  %4 = load i32, i32* %arrayidx1.epil, align 4
  %cmp2.epil = icmp slt i32 %4, 10
  br i1 %cmp2.epil, label %if.then.epil, label %if.else.epil

if.else.epil:                                     ; preds = %for.body.epil
  %idxprom6.epil = zext i32 %i.02.unr to i64
  %arrayidx8.epil = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom6.epil, i64 0
  store i32 0, i32* %arrayidx8.epil, align 4
  br label %for.inc.epil

if.then.epil:                                     ; preds = %for.body.epil
  %idxprom3.epil = zext i32 %i.02.unr to i64
  %arrayidx5.epil = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom3.epil, i64 0
  store i32 1, i32* %arrayidx5.epil, align 4
  br label %for.inc.epil

for.inc.epil:                                     ; preds = %if.then.epil, %if.else.epil
  br label %for.cond.for.end_crit_edge

for.cond.for.end_crit_edge:                       ; preds = %for.cond.for.end_crit_edge.unr-lcssa, %for.inc.epil
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %entry
  ret void
}

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
