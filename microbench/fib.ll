; ModuleID = 'fib.bc'
source_filename = "fib.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }
%struct.timeval = type { i64, i64 }
%struct.timezone = type { i32, i32 }

@stderr = external dso_local global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [51 x i8] c"Usage: fib [<cilk options>] <n> <number of round>\0A\00", align 1
@.str.1 = private unnamed_addr constant [15 x i8] c"PBBS-time: %f\0A\00", align 1
@.str.2 = private unnamed_addr constant [12 x i8] c"Result: %d\0A\00", align 1

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local noundef i64 @_Z6todvalP7timeval(%struct.timeval* noundef %tp) #0 {
entry:
  %tv_sec = getelementptr inbounds %struct.timeval, %struct.timeval* %tp, i64 0, i32 0
  %0 = load i64, i64* %tv_sec, align 8
  %mul1 = mul i64 %0, 1000000
  %tv_usec = getelementptr inbounds %struct.timeval, %struct.timeval* %tp, i64 0, i32 1
  %1 = load i64, i64* %tv_usec, align 8
  %add = add nsw i64 %mul1, %1
  ret i64 %add
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z12startup_cilkv() #1 {
entry:
  %g = alloca double, align 8
  %syncreg = call token @llvm.syncregion.start()
  store double 2.000000e+00, double* %g, align 8
  br i1 true, label %pfor.ph, label %pfor.end

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
  %0 = load double, double* %g, align 8
  %inc = fadd double %0, 1.000000e+00
  store double %inc, double* %g, align 8
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc3 = add nuw nsw i32 %__begin.0, 1
  %cmp4 = icmp ult i32 %__begin.0, 999
  br i1 %cmp4, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %entry
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #2

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #3

; Function Attrs: mustprogress noinline uwtable
define dso_local noundef i32 @_Z3fibi(i32 noundef %n) #1 {
entry:
  %x = alloca i32, align 4
  %syncreg = call token @llvm.syncregion.start()
  %cmp = icmp slt i32 %n, 2
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %return

if.else:                                          ; preds = %entry
  br label %if.else.tf

if.else.tf:                                       ; preds = %if.else
  %0 = call token @llvm.taskframe.create()
  detach within %syncreg, label %det.achd, label %det.cont

det.achd:                                         ; preds = %if.else.tf
  %sub = add nsw i32 %n, -1
  call void @llvm.taskframe.use(token %0)
  %call = call noundef i32 @_Z3fibi(i32 noundef %sub)
  store i32 %call, i32* %x, align 4
  reattach within %syncreg, label %det.cont

det.cont:                                         ; preds = %det.achd, %if.else.tf
  %sub1 = add nsw i32 %n, -2
  %call2 = call noundef i32 @_Z3fibi(i32 noundef %sub1)
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %det.cont
  call void @llvm.sync.unwind(token %syncreg)
  %1 = load i32, i32* %x, align 4
  %add = add nsw i32 %1, %call2
  sync within %syncreg, label %sync.continue3

sync.continue3:                                   ; preds = %sync.continue
  call void @llvm.sync.unwind(token %syncreg)
  br label %return

return:                                           ; preds = %sync.continue3, %if.then
  %retval.0 = phi i32 [ %n, %if.then ], [ %add, %sync.continue3 ]
  sync within %syncreg, label %sync.continue4

sync.continue4:                                   ; preds = %return
  call void @llvm.sync.unwind(token %syncreg)
  ret i32 %retval.0
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.taskframe.create() #2

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.taskframe.use(token) #2

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main(i32 noundef %argc, i8** noundef %argv) #4 {
entry:
  %t1 = alloca %struct.timeval, align 8
  %t2 = alloca %struct.timeval, align 8
  %cmp.not = icmp eq i32 %argc, 3
  br i1 %cmp.not, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  %0 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8
  %1 = call i64 @fwrite(i8* getelementptr inbounds ([51 x i8], [51 x i8]* @.str, i64 0, i64 0), i64 50, i64 1, %struct._IO_FILE* %0) #10
  call void @exit(i32 noundef 1) #11
  unreachable

if.end:                                           ; preds = %entry
  call void @_Z12startup_cilkv()
  %arrayidx = getelementptr inbounds i8*, i8** %argv, i64 1
  %2 = load i8*, i8** %arrayidx, align 8
  %call1 = call i32 @atoi(i8* noundef %2) #12
  %arrayidx2 = getelementptr inbounds i8*, i8** %argv, i64 2
  %3 = load i8*, i8** %arrayidx2, align 8
  %call3 = call i32 @atoi(i8* noundef %3) #12
  %cmp41 = icmp sgt i32 %call3, 0
  br i1 %cmp41, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %if.end
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %r.02 = phi i32 [ 0, %for.body.lr.ph ], [ %inc, %for.inc ]
  %call5 = call i32 @gettimeofday(%struct.timeval* noundef nonnull %t1, %struct.timezone* noundef null) #13
  %call6 = call noundef i32 @_Z3fibi(i32 noundef %call1)
  %call7 = call i32 @gettimeofday(%struct.timeval* noundef nonnull %t2, %struct.timezone* noundef null) #13
  %call8 = call noundef i64 @_Z6todvalP7timeval(%struct.timeval* noundef nonnull %t2)
  %call9 = call noundef i64 @_Z6todvalP7timeval(%struct.timeval* noundef nonnull %t1)
  %sub = sub i64 %call8, %call9
  %div = udiv i64 %sub, 1000
  %conv = uitofp i64 %div to double
  %div10 = fdiv double %conv, 1.000000e+03
  %call11 = call i32 (i8*, ...) @printf(i8* noundef nonnull dereferenceable(1) getelementptr inbounds ([15 x i8], [15 x i8]* @.str.1, i64 0, i64 0), double noundef %div10)
  %4 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8
  %call12 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* noundef %4, i8* noundef getelementptr inbounds ([12 x i8], [12 x i8]* @.str.2, i64 0, i64 0), i32 noundef %call6) #10
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %r.02, 1
  %cmp4 = icmp slt i32 %inc, %call3
  br i1 %cmp4, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !6

for.cond.for.end_crit_edge:                       ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %if.end
  ret i32 0
}

declare dso_local i32 @fprintf(%struct._IO_FILE* noundef, i8* noundef, ...) #5

; Function Attrs: noreturn nounwind
declare dso_local void @exit(i32 noundef) #6

; Function Attrs: nounwind readonly willreturn
declare dso_local i32 @atoi(i8* noundef) #7

; Function Attrs: nounwind
declare dso_local i32 @gettimeofday(%struct.timeval* noundef, %struct.timezone* noundef) #8

declare dso_local i32 @printf(i8* noundef, ...) #5

; Function Attrs: nofree nounwind
declare noundef i64 @fwrite(i8* nocapture noundef, i64 noundef, i64 noundef, %struct._IO_FILE* nocapture noundef) #9

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { argmemonly nounwind willreturn }
attributes #3 = { argmemonly willreturn }
attributes #4 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { noreturn nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind readonly willreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { nofree nounwind }
attributes #10 = { cold }
attributes #11 = { noreturn nounwind }
attributes #12 = { nounwind readonly willreturn }
attributes #13 = { nounwind }

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
