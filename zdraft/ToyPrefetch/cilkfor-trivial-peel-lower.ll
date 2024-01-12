; ModuleID = 'cilkfor-trivial-peel.bc'
source_filename = "cilkfor-trivial.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.__cilkrts_stack_frame = type { i32, i32, %struct.__cilkrts_stack_frame*, %struct.__cilkrts_worker*, i8*, [5 x i8*], i32, i16, i16, { %struct.__cilkrts_pedigree } }
%struct.__cilkrts_worker = type { %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, i32, i8*, i8*, i8*, %struct.__cilkrts_stack_frame*, i8*, i8*, %struct.__cilkrts_pedigree }
%struct.__cilkrts_pedigree = type { i64, %struct.__cilkrts_pedigree* }

; Function Attrs: noinline stealable uwtable
define dso_local void @_Z4loopPii(i32* noundef %A, i32 noundef %N) #0 {
entry:
  %__cilkrts_sf = alloca %struct.__cilkrts_stack_frame, align 8
  %0 = call %struct.__cilkrts_worker* @__cilkrts_get_tls_worker() #4
  %1 = icmp eq %struct.__cilkrts_worker* %0, null
  br i1 %1, label %slowpath.i, label %fastpath.i

slowpath.i:                                       ; preds = %entry
  %2 = call %struct.__cilkrts_worker* @__cilkrts_bind_thread_1() #4
  %3 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store atomic i32 16777344, i32* %3 release, align 4
  br label %__cilkrts_enter_frame_1.exit

fastpath.i:                                       ; preds = %entry
  %4 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store atomic i32 16777216, i32* %4 release, align 4
  br label %__cilkrts_enter_frame_1.exit

__cilkrts_enter_frame_1.exit:                     ; preds = %slowpath.i, %fastpath.i
  %5 = phi %struct.__cilkrts_worker* [ %2, %slowpath.i ], [ %0, %fastpath.i ]
  %6 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %5, i32 0, i32 9
  %7 = load atomic %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %6 acquire, align 8
  %8 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store atomic %struct.__cilkrts_stack_frame* %7, %struct.__cilkrts_stack_frame** %8 release, align 8
  %9 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  store atomic %struct.__cilkrts_worker* %5, %struct.__cilkrts_worker** %9 release, align 8
  %10 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %5, i32 0, i32 9
  store atomic %struct.__cilkrts_stack_frame* %__cilkrts_sf, %struct.__cilkrts_stack_frame** %10 release, align 8
  %syncreg = call token @llvm.syncregion.start()
  %cmp = icmp sgt i32 %N, 0
  br i1 %cmp, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %__cilkrts_enter_frame_1.exit
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc3, %pfor.inc ]
  %11 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 6
  %12 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 7
  call void asm sideeffect "stmxcsr $0\0A\09fnstcw $1", "*m,*m,~{dirflag},~{fpsr},~{flags}"(i32* elementtype(i32) %11, i16* elementtype(i16) %12)
  %13 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 5
  %14 = call i8* @llvm.frameaddress.p0i8(i32 0)
  %15 = getelementptr inbounds [5 x i8*], [5 x i8*]* %13, i32 0, i32 0
  store volatile i8* %14, i8** %15, align 8
  %16 = call i8* @llvm.stacksave()
  %17 = getelementptr inbounds [5 x i8*], [5 x i8*]* %13, i32 0, i32 2
  store volatile i8* %16, i8** %17, align 8
  %18 = bitcast [5 x i8*]* %13 to i8*
  %19 = call i32 @llvm.eh.sjlj.setjmp(i8* %18) #7
  %20 = icmp eq i32 %19, 0
  br i1 %20, label %pfor.cond.split, label %pfor.inc

pfor.cond.split:                                  ; preds = %pfor.cond
  call fastcc void @_Z4loopPii.outline_pfor.body.otd1(i32* %A, i32 %__begin.0)
  br label %pfor.inc

pfor.body:                                        ; No predecessors!
  %idxprom = zext i32 %__begin.0 to i64
  %arrayidx = getelementptr inbounds i32, i32* %A, i64 %idxprom
  %21 = load i32, i32* %arrayidx, align 4
  %inc = add nsw i32 %21, 1
  store i32 %inc, i32* %arrayidx, align 4
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.cond, %pfor.cond.split, %pfor.body
  %inc3 = add nuw nsw i32 %__begin.0, 1
  %cmp4 = icmp slt i32 %inc3, %N
  br i1 %cmp4, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  %22 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %23 = load atomic i32, i32* %22 acquire, align 4
  %24 = and i32 %23, 2
  %25 = icmp eq i32 %24, 0
  br i1 %25, label %__cilk_sync.exit, label %cilk.sync.savestate.i

cilk.sync.savestate.i:                            ; preds = %pfor.cond.cleanup
  %26 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %27 = load atomic %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %26 acquire, align 8
  %28 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %27, i32 0, i32 12
  %29 = load %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %28, align 8
  %30 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  %31 = load { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %30, align 8
  %32 = insertvalue { %struct.__cilkrts_pedigree } %31, %struct.__cilkrts_pedigree %29, 0
  %33 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  store { %struct.__cilkrts_pedigree } %32, { %struct.__cilkrts_pedigree }* %33, align 8
  %34 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 6
  %35 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 7
  call void asm sideeffect "stmxcsr $0\0A\09fnstcw $1", "*m,*m,~{dirflag},~{fpsr},~{flags}"(i32* elementtype(i32) %34, i16* elementtype(i16) %35)
  %36 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 5
  %37 = call i8* @llvm.frameaddress.p0i8(i32 0)
  %38 = getelementptr inbounds [5 x i8*], [5 x i8*]* %36, i32 0, i32 0
  store volatile i8* %37, i8** %38, align 8
  %39 = call i8* @llvm.stacksave()
  %40 = getelementptr inbounds [5 x i8*], [5 x i8*]* %36, i32 0, i32 2
  store volatile i8* %39, i8** %40, align 8
  %41 = bitcast [5 x i8*]* %36 to i8*
  %42 = call i32 @llvm.eh.sjlj.setjmp(i8* %41) #7
  %43 = icmp eq i32 %42, 0
  br i1 %43, label %cilk.sync.runtimecall.i, label %cilk.sync.excepting.i

cilk.sync.runtimecall.i:                          ; preds = %cilk.sync.savestate.i
  call void @__cilkrts_sync(%struct.__cilkrts_stack_frame* %__cilkrts_sf)
  br label %__cilk_sync.exit

cilk.sync.excepting.i:                            ; preds = %cilk.sync.savestate.i
  %44 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %45 = load atomic i32, i32* %44 acquire, align 4
  %46 = and i32 %45, 16
  %47 = icmp eq i32 %46, 0
  br i1 %47, label %__cilk_sync.exit, label %cilk.sync.rethrow.i

cilk.sync.rethrow.i:                              ; preds = %cilk.sync.excepting.i
  call void @__cilkrts_rethrow(%struct.__cilkrts_stack_frame* %__cilkrts_sf) #8
  unreachable

__cilk_sync.exit:                                 ; preds = %pfor.cond.cleanup, %cilk.sync.runtimecall.i, %cilk.sync.excepting.i
  %48 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %49 = load atomic %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %48 acquire, align 8
  %50 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %49, i32 0, i32 12
  %51 = getelementptr inbounds %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %50, i32 0, i32 0
  %52 = load i64, i64* %51, align 8
  %53 = add i64 %52, 1
  store i64 %53, i64* %51, align 8
  br label %sync.continue

sync.continue:                                    ; preds = %__cilk_sync.exit
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %__cilkrts_enter_frame_1.exit
  %54 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %55 = load atomic %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %54 acquire, align 8
  %56 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %57 = load %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %56, align 8
  %58 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %55, i32 0, i32 9
  store atomic %struct.__cilkrts_stack_frame* %57, %struct.__cilkrts_stack_frame** %58 release, align 8
  %59 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store atomic %struct.__cilkrts_stack_frame* null, %struct.__cilkrts_stack_frame** %59 release, align 8
  %60 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %61 = load atomic i32, i32* %60 acquire, align 4
  %62 = icmp ne i32 %61, 16777216
  br i1 %62, label %body.i, label %__cilk_parent_epilogue.exit

body.i:                                           ; preds = %pfor.end
  call void @__cilkrts_leave_frame(%struct.__cilkrts_stack_frame* %__cilkrts_sf) #4
  br label %__cilk_parent_epilogue.exit

__cilk_parent_epilogue.exit:                      ; preds = %pfor.end, %body.i
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #1

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #2

; Function Attrs: noinline uwtable
define private fastcc void @_Z4loopPii.outline_pfor.body.otd1(i32* noundef align 1 %A.otd1, i32 %__begin.0.otd1) unnamed_addr #3 {
pfor.cond.otd1:
  %__cilkrts_sf = alloca %struct.__cilkrts_stack_frame, align 8
  %0 = call %struct.__cilkrts_worker* @__cilkrts_get_tls_worker_fast() #4
  %1 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store atomic i32 16777216, i32* %1 release, align 4
  %2 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %0, i32 0, i32 9
  %3 = load atomic %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %2 acquire, align 8
  %4 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store atomic %struct.__cilkrts_stack_frame* %3, %struct.__cilkrts_stack_frame** %4 release, align 8
  %5 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  store atomic %struct.__cilkrts_worker* %0, %struct.__cilkrts_worker** %5 release, align 8
  %6 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %0, i32 0, i32 9
  store atomic %struct.__cilkrts_stack_frame* %__cilkrts_sf, %struct.__cilkrts_stack_frame** %6 release, align 8
  %7 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %8 = load %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %7, align 8
  %9 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %10 = load %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %9, align 8
  %11 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 0
  %12 = load atomic %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame*** %11 acquire, align 8
  %13 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %14 = load %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %13, align 8
  %15 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  %16 = load { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %15, align 8
  %17 = insertvalue { %struct.__cilkrts_pedigree } %16, %struct.__cilkrts_pedigree %14, 0
  %18 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  store { %struct.__cilkrts_pedigree } %17, { %struct.__cilkrts_pedigree }* %18, align 8
  %19 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %10, i32 0, i32 9
  %20 = load { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %19, align 8
  %21 = insertvalue { %struct.__cilkrts_pedigree } %20, %struct.__cilkrts_pedigree %14, 0
  %22 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %10, i32 0, i32 9
  store { %struct.__cilkrts_pedigree } %21, { %struct.__cilkrts_pedigree }* %22, align 8
  %23 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %24 = getelementptr inbounds %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %23, i32 0, i32 0
  store atomic i64 0, i64* %24 release, align 8
  %25 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %26 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  %27 = getelementptr inbounds { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %26, i32 0, i32 0
  %28 = getelementptr inbounds %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %25, i32 0, i32 1
  store atomic %struct.__cilkrts_pedigree* %27, %struct.__cilkrts_pedigree** %28 release, align 8
  fence release
  store volatile %struct.__cilkrts_stack_frame* %10, %struct.__cilkrts_stack_frame** %12, align 8
  %29 = getelementptr %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %12, i32 1
  %30 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 0
  store atomic %struct.__cilkrts_stack_frame** %29, %struct.__cilkrts_stack_frame*** %30 release, align 8
  %31 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %32 = load atomic i32, i32* %31 acquire, align 4
  %33 = or i32 %32, 4
  %34 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store atomic i32 %33, i32* %34 release, align 4
  br label %pfor.body.otd1

pfor.body.otd1:                                   ; preds = %pfor.cond.otd1
  %idxprom.otd1 = zext i32 %__begin.0.otd1 to i64
  %arrayidx.otd1 = getelementptr inbounds i32, i32* %A.otd1, i64 %idxprom.otd1
  %35 = load i32, i32* %arrayidx.otd1, align 4
  %inc.otd1 = add nsw i32 %35, 1
  store i32 %inc.otd1, i32* %arrayidx.otd1, align 4
  br label %pfor.inc.otd1

pfor.inc.otd1:                                    ; preds = %pfor.body.otd1
  %36 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %37 = load atomic %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %36 acquire, align 8
  %38 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %39 = load %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %38, align 8
  %40 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %37, i32 0, i32 9
  store atomic %struct.__cilkrts_stack_frame* %39, %struct.__cilkrts_stack_frame** %40 release, align 8
  %41 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store atomic %struct.__cilkrts_stack_frame* null, %struct.__cilkrts_stack_frame** %41 release, align 8
  %42 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %43 = load atomic i32, i32* %42 acquire, align 4
  %44 = icmp ne i32 %43, 16777216
  br i1 %44, label %body.i, label %__cilk_parent_epilogue.exit

body.i:                                           ; preds = %pfor.inc.otd1
  call void @__cilkrts_leave_frame(%struct.__cilkrts_stack_frame* %__cilkrts_sf) #4
  br label %__cilk_parent_epilogue.exit

__cilk_parent_epilogue.exit:                      ; preds = %pfor.inc.otd1, %body.i
  ret void
}

; Function Attrs: nounwind
declare %struct.__cilkrts_worker* @__cilkrts_get_tls_worker_fast() #4

; Function Attrs: nounwind
declare void @__cilkrts_leave_frame(%struct.__cilkrts_stack_frame*) #4

; Function Attrs: nounwind
declare %struct.__cilkrts_worker* @__cilkrts_get_tls_worker() #4

; Function Attrs: nounwind
declare %struct.__cilkrts_worker* @__cilkrts_bind_thread_1() #4

; Function Attrs: nofree nosync nounwind readnone willreturn
declare i8* @llvm.frameaddress.p0i8(i32 immarg) #5

; Function Attrs: nofree nosync nounwind willreturn
declare i8* @llvm.stacksave() #6

; Function Attrs: nounwind
declare i32 @llvm.eh.sjlj.setjmp(i8*) #4

declare void @__cilkrts_sync(%struct.__cilkrts_stack_frame*)

declare void @__cilkrts_rethrow(%struct.__cilkrts_stack_frame*)

attributes #0 = { noinline stealable uwtable "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { argmemonly willreturn }
attributes #3 = { noinline uwtable "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }
attributes #5 = { nofree nosync nounwind readnone willreturn }
attributes #6 = { nofree nosync nounwind willreturn }
attributes #7 = { returns_twice }
attributes #8 = { noreturn }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"tapir.loop.spawn.strategy", i32 1}
