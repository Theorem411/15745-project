; ModuleID = 'for-simple.bc'
source_filename = "for-simple.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z4loopPA10_iS0_i([10 x i32]* noundef %A, [10 x i32]* noundef %B, i32 noundef %N) #0 {
entry:
  %cmp1 = icmp sgt i32 %N, 0
  br i1 %cmp1, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %entry
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %i.02 = phi i32 [ 0, %for.body.lr.ph ], [ %inc, %for.inc ]
  %idxprom = zext i32 %i.02 to i64
  %arrayidx1 = getelementptr inbounds [10 x i32], [10 x i32]* %B, i64 0, i64 %idxprom
  %0 = load i32, i32* %arrayidx1, align 4
  %cmp2 = icmp slt i32 %0, 10
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

for.inc:                                          ; preds = %if.then, %if.else
  %inc = add nuw nsw i32 %i.02, 1
  %cmp = icmp slt i32 %inc, %N
  br i1 %cmp, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !4

for.cond.for.end_crit_edge:                       ; preds = %for.inc
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
