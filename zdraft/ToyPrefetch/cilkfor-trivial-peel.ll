; ModuleID = 'cilkfor-trivial-peel.bc'
source_filename = "cilkfor-trivial.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4loopPii(i32* noundef %A, i32 noundef %N) #0 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  %cmp = icmp sgt i32 %N, 0
  br i1 %cmp, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %entry
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc3, %pfor.inc ]
  detach within %syncreg, label %pfor.body, label %pfor.inc

pfor.body:                                        ; preds = %pfor.cond
  %idxprom = zext i32 %__begin.0 to i64
  %arrayidx = getelementptr inbounds i32, i32* %A, i64 %idxprom
  %0 = load i32, i32* %arrayidx, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, i32* %arrayidx, align 4
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.body, %pfor.cond
  %inc3 = add nuw nsw i32 %__begin.0, 1
  %cmp4 = icmp slt i32 %inc3, %N
  br i1 %cmp4, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %entry
  %cmp61 = icmp sgt i32 %N, 0
  br i1 %cmp61, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %pfor.end
  %1 = add i32 %N, -1
  %2 = urem i32 %1, 3
  %3 = add i32 %2, 1
  %xtraiter = urem i32 %3, 3
  %4 = icmp ult i32 %1, 2
  br i1 %4, label %for.cond.for.end_crit_edge.unr-lcssa, label %for.body.lr.ph.new

for.body.lr.ph.new:                               ; preds = %for.body.lr.ph
  %unroll_iter = sub i32 %N, %xtraiter
  br label %for.body

for.body:                                         ; preds = %for.inc.2, %for.body.lr.ph.new
  %i5.02 = phi i32 [ 0, %for.body.lr.ph.new ], [ %inc11.2, %for.inc.2 ]
  %niter = phi i32 [ 0, %for.body.lr.ph.new ], [ %niter.next.2, %for.inc.2 ]
  %cmp7 = icmp slt i32 %N, 10
  br i1 %cmp7, label %if.then, label %for.inc

if.then:                                          ; preds = %for.body
  %idxprom8 = zext i32 %i5.02 to i64
  %arrayidx9 = getelementptr inbounds i32, i32* %A, i64 %idxprom8
  %5 = load i32, i32* %arrayidx9, align 4
  %inc10 = add nsw i32 %5, 1
  store i32 %inc10, i32* %arrayidx9, align 4
  br label %for.inc

for.inc:                                          ; preds = %if.then, %for.body
  %inc11 = add nuw nsw i32 %i5.02, 1
  %niter.next = add i32 %niter, 1
  %cmp7.1 = icmp slt i32 %N, 10
  br i1 %cmp7.1, label %if.then.1, label %for.inc.1

if.then.1:                                        ; preds = %for.inc
  %idxprom8.1 = zext i32 %inc11 to i64
  %arrayidx9.1 = getelementptr inbounds i32, i32* %A, i64 %idxprom8.1
  %6 = load i32, i32* %arrayidx9.1, align 4
  %inc10.1 = add nsw i32 %6, 1
  store i32 %inc10.1, i32* %arrayidx9.1, align 4
  br label %for.inc.1

for.inc.1:                                        ; preds = %if.then.1, %for.inc
  %inc11.1 = add nuw nsw i32 %inc11, 1
  %niter.next.1 = add i32 %niter.next, 1
  %cmp7.2 = icmp slt i32 %N, 10
  br i1 %cmp7.2, label %if.then.2, label %for.inc.2

if.then.2:                                        ; preds = %for.inc.1
  %idxprom8.2 = zext i32 %inc11.1 to i64
  %arrayidx9.2 = getelementptr inbounds i32, i32* %A, i64 %idxprom8.2
  %7 = load i32, i32* %arrayidx9.2, align 4
  %inc10.2 = add nsw i32 %7, 1
  store i32 %inc10.2, i32* %arrayidx9.2, align 4
  br label %for.inc.2

for.inc.2:                                        ; preds = %if.then.2, %for.inc.1
  %inc11.2 = add nuw nsw i32 %inc11.1, 1
  %niter.next.2 = add i32 %niter.next.1, 1
  %niter.ncmp.2 = icmp ne i32 %niter.next.2, %unroll_iter
  br i1 %niter.ncmp.2, label %for.body, label %for.cond.for.end_crit_edge.unr-lcssa.loopexit, !llvm.loop !6

for.cond.for.end_crit_edge.unr-lcssa.loopexit:    ; preds = %for.inc.2
  %i5.02.unr.ph = phi i32 [ %inc11.2, %for.inc.2 ]
  br label %for.cond.for.end_crit_edge.unr-lcssa

for.cond.for.end_crit_edge.unr-lcssa:             ; preds = %for.cond.for.end_crit_edge.unr-lcssa.loopexit, %for.body.lr.ph
  %i5.02.unr = phi i32 [ 0, %for.body.lr.ph ], [ %i5.02.unr.ph, %for.cond.for.end_crit_edge.unr-lcssa.loopexit ]
  %lcmp.mod = icmp ne i32 %xtraiter, 0
  br i1 %lcmp.mod, label %for.body.epil.preheader, label %for.cond.for.end_crit_edge

for.body.epil.preheader:                          ; preds = %for.cond.for.end_crit_edge.unr-lcssa
  br label %for.body.epil

for.body.epil:                                    ; preds = %for.inc.epil, %for.body.epil.preheader
  %i5.02.epil = phi i32 [ %i5.02.unr, %for.body.epil.preheader ], [ %inc11.epil, %for.inc.epil ]
  %epil.iter = phi i32 [ 0, %for.body.epil.preheader ], [ %epil.iter.next, %for.inc.epil ]
  %cmp7.epil = icmp slt i32 %N, 10
  br i1 %cmp7.epil, label %if.then.epil, label %for.inc.epil

if.then.epil:                                     ; preds = %for.body.epil
  %idxprom8.epil = zext i32 %i5.02.epil to i64
  %arrayidx9.epil = getelementptr inbounds i32, i32* %A, i64 %idxprom8.epil
  %8 = load i32, i32* %arrayidx9.epil, align 4
  %inc10.epil = add nsw i32 %8, 1
  store i32 %inc10.epil, i32* %arrayidx9.epil, align 4
  br label %for.inc.epil

for.inc.epil:                                     ; preds = %if.then.epil, %for.body.epil
  %inc11.epil = add nuw nsw i32 %i5.02.epil, 1
  %cmp6.epil = icmp slt i32 %inc11.epil, %N
  %epil.iter.next = add i32 %epil.iter, 1
  %epil.iter.cmp = icmp ne i32 %epil.iter.next, %xtraiter
  br i1 %epil.iter.cmp, label %for.body.epil, label %for.cond.for.end_crit_edge.epilog-lcssa, !llvm.loop !8

for.cond.for.end_crit_edge.epilog-lcssa:          ; preds = %for.inc.epil
  br label %for.cond.for.end_crit_edge

for.cond.for.end_crit_edge:                       ; preds = %for.cond.for.end_crit_edge.unr-lcssa, %for.cond.for.end_crit_edge.epilog-lcssa
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %pfor.end
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
!5 = !{!"tapir.loop.spawn.strategy", i32 1}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !9}
!9 = !{!"llvm.loop.unroll.disable"}
