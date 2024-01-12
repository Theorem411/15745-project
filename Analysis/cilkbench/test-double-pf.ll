; ModuleID = './cilkbench/test-double-pf.bc'
source_filename = "test-double.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z4loopPA10_iS0_([10 x i32]* noundef %A, [10 x i32]* noundef %B) #0 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  br i1 true, label %pfor.ph, label %pfor.end37

pfor.ph:                                          ; preds = %entry
  %.subst = getelementptr [10 x i32], [10 x i32]* %B, i64 1, i64 0
  %scevgep4 = bitcast i32* %.subst to i8*
  call void @llvm.prefetch.p0i8(i8* %scevgep4, i32 1, i32 1, i32 1)
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc32, %pfor.ph
  %indvar = phi i64 [ %indvar.next, %pfor.inc32 ], [ 0, %pfor.ph ]
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc33, %pfor.inc32 ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc32

pfor.body.entry:                                  ; preds = %pfor.detach
  %syncreg3 = call token @llvm.syncregion.start()
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  br i1 true, label %pfor.ph7, label %pfor.end

pfor.ph7:                                         ; preds = %pfor.body
  br label %pfor.cond14

pfor.cond14:                                      ; preds = %pfor.inc, %pfor.ph7
  %indvar1 = phi i64 [ %indvar.next2, %pfor.inc ], [ 0, %pfor.ph7 ]
  %__begin8.0 = phi i32 [ 0, %pfor.ph7 ], [ %inc, %pfor.inc ]
  %0 = add i64 %indvar1, 2
  %scevgep = getelementptr [10 x i32], [10 x i32]* %A, i64 %indvar, i64 %0
  %scevgep3 = bitcast i32* %scevgep to i8*
  call void @llvm.prefetch.p0i8(i8* %scevgep3, i32 0, i32 1, i32 1)
  br label %pfor.detach15

pfor.detach15:                                    ; preds = %pfor.cond14
  detach within %syncreg3, label %pfor.body.entry18, label %pfor.inc

pfor.body.entry18:                                ; preds = %pfor.detach15
  br label %pfor.body19

pfor.body19:                                      ; preds = %pfor.body.entry18
  %idxprom = zext i32 %__begin8.0 to i64
  %arrayidx20 = getelementptr inbounds [10 x i32], [10 x i32]* %B, i64 %idxprom, i64 0
  %1 = load i32, i32* %arrayidx20, align 4
  %add21 = add nuw nsw i32 %__begin8.0, 1
  %idxprom22 = zext i32 %add21 to i64
  %arrayidx24 = getelementptr inbounds [10 x i32], [10 x i32]* %B, i64 %idxprom22, i64 0
  %2 = load i32, i32* %arrayidx24, align 4
  %add25 = add nsw i32 %1, %2
  %idxprom26 = zext i32 %__begin.0 to i64
  %idxprom28 = zext i32 %__begin8.0 to i64
  %arrayidx29 = getelementptr inbounds [10 x i32], [10 x i32]* %A, i64 %idxprom26, i64 %idxprom28
  store i32 %add25, i32* %arrayidx29, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body19
  reattach within %syncreg3, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach15
  %inc = add nuw nsw i32 %__begin8.0, 1
  %cmp30 = icmp ult i32 %__begin8.0, 9
  %indvar.next2 = add i64 %indvar1, 1
  br i1 %cmp30, label %pfor.cond14, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg3, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg3)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %pfor.body
  br label %pfor.preattach31

pfor.preattach31:                                 ; preds = %pfor.end
  reattach within %syncreg, label %pfor.inc32

pfor.inc32:                                       ; preds = %pfor.preattach31, %pfor.detach
  %inc33 = add nuw nsw i32 %__begin.0, 1
  %cmp34 = icmp ult i32 %__begin.0, 2
  %indvar.next = add i64 %indvar, 1
  br i1 %cmp34, label %pfor.cond, label %pfor.cond.cleanup35, !llvm.loop !6

pfor.cond.cleanup35:                              ; preds = %pfor.inc32
  sync within %syncreg, label %sync.continue36

sync.continue36:                                  ; preds = %pfor.cond.cleanup35
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end37

pfor.end37:                                       ; preds = %sync.continue36, %entry
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #1

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #2

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main() #3 {
entry:
  %A = alloca [3 x [10 x i32]], align 16
  %B = alloca [3 x [10 x i32]], align 16
  br label %for.body

for.body:                                         ; preds = %for.inc6, %entry
  %indvar = phi i64 [ %indvar.next, %for.inc6 ], [ 0, %entry ]
  %i.02 = phi i32 [ 0, %entry ], [ %inc7, %for.inc6 ]
  br label %for.body3

for.body3:                                        ; preds = %for.inc, %for.body
  %indvar1 = phi i64 [ %indvar.next2, %for.inc ], [ 0, %for.body ]
  %j.01 = phi i32 [ 0, %for.body ], [ %inc, %for.inc ]
  %0 = add i64 %indvar1, 5
  %scevgep = getelementptr [3 x [10 x i32]], [3 x [10 x i32]]* %A, i64 0, i64 %indvar, i64 %0
  %scevgep3 = bitcast i32* %scevgep to i8*
  call void @llvm.prefetch.p0i8(i8* %scevgep3, i32 0, i32 1, i32 1)
  %idxprom = zext i32 %i.02 to i64
  %idxprom4 = zext i32 %j.01 to i64
  %arrayidx5 = getelementptr inbounds [3 x [10 x i32]], [3 x [10 x i32]]* %A, i64 0, i64 %idxprom, i64 %idxprom4
  store i32 0, i32* %arrayidx5, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body3
  %inc = add nuw nsw i32 %j.01, 1
  %cmp2 = icmp ult i32 %j.01, 9
  %indvar.next2 = add i64 %indvar1, 1
  br i1 %cmp2, label %for.body3, label %for.end, !llvm.loop !7

for.end:                                          ; preds = %for.inc
  br label %for.inc6

for.inc6:                                         ; preds = %for.end
  %inc7 = add nuw nsw i32 %i.02, 1
  %cmp = icmp ult i32 %i.02, 2
  %indvar.next = add i64 %indvar, 1
  br i1 %cmp, label %for.body, label %for.end8, !llvm.loop !9

for.end8:                                         ; preds = %for.inc6
  %arraydecay = getelementptr inbounds [3 x [10 x i32]], [3 x [10 x i32]]* %A, i64 0, i64 0
  %arraydecay9 = getelementptr inbounds [3 x [10 x i32]], [3 x [10 x i32]]* %B, i64 0, i64 0
  call void @_Z4loopPA10_iS0_([10 x i32]* noundef nonnull %arraydecay, [10 x i32]* noundef nonnull %arraydecay9)
  ret i32 0
}

; Function Attrs: inaccessiblemem_or_argmemonly nofree nosync nounwind willreturn
declare void @llvm.prefetch.p0i8(i8* nocapture readonly, i32 immarg, i32 immarg, i32) #4

attributes #0 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { argmemonly willreturn }
attributes #3 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { inaccessiblemem_or_argmemonly nofree nosync nounwind willreturn }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"tapir.loop.spawn.strategy", i32 1}
!6 = distinct !{!6, !5}
!7 = distinct !{!7, !8}
!8 = !{!"llvm.loop.mustprogress"}
!9 = distinct !{!9, !8}
