; ModuleID = 'cilkfor-trivial.bc'
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
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %idxprom = zext i32 %__begin.0 to i64
  %arrayidx = getelementptr inbounds i32, i32* %A, i64 %idxprom
  %0 = load i32, i32* %arrayidx, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, i32* %arrayidx, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
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
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %i5.02 = phi i32 [ 0, %for.body.lr.ph ], [ %inc11, %for.inc ]
  %cmp7 = icmp slt i32 %N, 10
  br i1 %cmp7, label %if.then, label %if.end

if.then:                                          ; preds = %for.body
  %idxprom8 = zext i32 %i5.02 to i64
  %arrayidx9 = getelementptr inbounds i32, i32* %A, i64 %idxprom8
  %1 = load i32, i32* %arrayidx9, align 4
  %inc10 = add nsw i32 %1, 1
  store i32 %inc10, i32* %arrayidx9, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %for.body
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %inc11 = add nuw nsw i32 %i5.02, 1
  %cmp6 = icmp slt i32 %inc11, %N
  br i1 %cmp6, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !6

for.cond.for.end_crit_edge:                       ; preds = %for.inc
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
