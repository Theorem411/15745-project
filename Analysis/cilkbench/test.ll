; ModuleID = 'test.bc'
source_filename = "test.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4loopPiS_(i32* noundef %A, i32* noundef %B) #0 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  br i1 true, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %entry
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %idxprom = zext i32 %__begin.0 to i64
  %arrayidx = getelementptr inbounds i32, i32* %B, i64 %idxprom
  %0 = load i32, i32* %arrayidx, align 4
  %add3 = add nuw nsw i32 %__begin.0, 1
  %idxprom4 = zext i32 %add3 to i64
  %arrayidx5 = getelementptr inbounds i32, i32* %B, i64 %idxprom4
  %1 = load i32, i32* %arrayidx5, align 4
  %add6 = add nsw i32 %0, %1
  %idxprom7 = zext i32 %__begin.0 to i64
  %arrayidx8 = getelementptr inbounds i32, i32* %A, i64 %idxprom7
  store i32 %add6, i32* %arrayidx8, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc = add nuw nsw i32 %__begin.0, 1
  %cmp9 = icmp ult i32 %__begin.0, 9
  br i1 %cmp9, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %entry
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
