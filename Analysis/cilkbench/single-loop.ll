; ModuleID = 'single-loop.bc'
source_filename = "single-loop.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }
%struct.timeval = type { i64, i64 }
%"struct.std::chrono::time_point" = type { %"struct.std::chrono::duration" }
%"struct.std::chrono::duration" = type { i64 }
%"struct.std::chrono::duration.0" = type { i64 }

$_ZNSt6chrono13duration_castINS_8durationIlSt5ratioILl1ELl1000000EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE = comdat any

$_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE = comdat any

$_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000EEE5countEv = comdat any

$_ZNSt6chrono20__duration_cast_implINS_8durationIlSt5ratioILl1ELl1000000EEEES2_ILl1ELl1000EElLb1ELb0EE6__castIlS2_ILl1ELl1000000000EEEES4_RKNS1_IT_T0_EE = comdat any

$_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv = comdat any

$_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000EEEC2IlvEERKT_ = comdat any

$_ZNSt6chronomiIlSt5ratioILl1ELl1000000000EElS2_EENSt11common_typeIJNS_8durationIT_T0_EENS4_IT1_T2_EEEE4typeERKS7_RKSA_ = comdat any

$_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv = comdat any

$_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_ = comdat any

@ARRAY_SIZE = dso_local global i32 50000, align 4
@stderr = external dso_local global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [56 x i8] c"Usage: simple-loop [<cilk options>] <number of rounds>\0A\00", align 1
@.str.1 = private unnamed_addr constant [30 x i8] c"simple-loop Time: %f seconds\0A\00", align 1
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
define dso_local noundef i32 @_Z10simpleLoopPi(i32* noundef %A) #1 {
entry:
  %result = alloca i32, align 4
  %syncreg = call token @llvm.syncregion.start()
  store i32 0, i32* %result, align 4
  %0 = load i32, i32* @ARRAY_SIZE, align 4
  %cmp = icmp sgt i32 %0, 0
  br i1 %cmp, label %pfor.ph, label %pfor.end

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
  %arrayidx = getelementptr inbounds i32, i32* %A, i64 %idxprom
  %1 = load i32, i32* %arrayidx, align 4
  %2 = load i32, i32* %result, align 4
  %add3 = add nsw i32 %2, %1
  store i32 %add3, i32* %result, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc = add nuw nsw i32 %__begin.0, 1
  %cmp4 = icmp slt i32 %inc, %0
  br i1 %cmp4, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !6

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %entry
  %3 = load i32, i32* %result, align 4
  ret i32 %3
}

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main(i32 noundef %argc, i8** noundef %argv) #4 {
entry:
  %start_time = alloca %"struct.std::chrono::time_point", align 8
  %end_time = alloca %"struct.std::chrono::time_point", align 8
  %ref.tmp = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp14 = alloca %"struct.std::chrono::duration", align 8
  %cmp.not = icmp eq i32 %argc, 2
  br i1 %cmp.not, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  %0 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8
  %1 = call i64 @fwrite(i8* getelementptr inbounds ([56 x i8], [56 x i8]* @.str, i64 0, i64 0), i64 55, i64 1, %struct._IO_FILE* %0) #12
  call void @exit(i32 noundef 1) #13
  unreachable

if.end:                                           ; preds = %entry
  call void @_Z12startup_cilkv()
  %arrayidx = getelementptr inbounds i8*, i8** %argv, i64 1
  %2 = load i8*, i8** %arrayidx, align 8
  %call1 = call i32 @atoi(i8* noundef %2) #14
  %3 = load i32, i32* @ARRAY_SIZE, align 4
  %conv = sext i32 %3 to i64
  %call2 = call noalias i8* @calloc(i64 noundef %conv, i64 noundef 8) #15
  %4 = bitcast i8* %call2 to i32*
  %cmp31 = icmp sgt i32 %3, 0
  br i1 %cmp31, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %if.end
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %i.02 = phi i32 [ 0, %for.body.lr.ph ], [ %inc, %for.inc ]
  %idxprom = zext i32 %i.02 to i64
  %arrayidx4 = getelementptr inbounds i32, i32* %4, i64 %idxprom
  store i32 %i.02, i32* %arrayidx4, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %i.02, 1
  %5 = load i32, i32* @ARRAY_SIZE, align 4
  %cmp3 = icmp slt i32 %inc, %5
  br i1 %cmp3, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !7

for.cond.for.end_crit_edge:                       ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %if.end
  %cmp63 = icmp sgt i32 %call1, 0
  br i1 %cmp63, label %for.body7.lr.ph, label %for.end25

for.body7.lr.ph:                                  ; preds = %for.end
  br label %for.body7

for.body7:                                        ; preds = %for.body7.lr.ph, %for.inc23
  %r.04 = phi i32 [ 0, %for.body7.lr.ph ], [ %inc24, %for.inc23 ]
  %call8 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #15
  %coerce.dive9 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %start_time, i64 0, i32 0, i32 0
  store i64 %call8, i64* %coerce.dive9, align 8
  %call10 = call noundef i32 @_Z10simpleLoopPi(i32* noundef %4)
  %call11 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #15
  %coerce.dive13 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %end_time, i64 0, i32 0, i32 0
  store i64 %call11, i64* %coerce.dive13, align 8
  %call15 = call i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %end_time, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %start_time)
  %coerce.dive16 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp14, i64 0, i32 0
  store i64 %call15, i64* %coerce.dive16, align 8
  %call17 = call i64 @_ZNSt6chrono13duration_castINS_8durationIlSt5ratioILl1ELl1000000EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp14)
  %coerce.dive18 = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %ref.tmp, i64 0, i32 0
  store i64 %call17, i64* %coerce.dive18, align 8
  %call19 = call noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %conv20 = sitofp i64 %call19 to double
  %div = fdiv double %conv20, 1.000000e+06
  %call21 = call i32 (i8*, ...) @printf(i8* noundef nonnull dereferenceable(1) getelementptr inbounds ([30 x i8], [30 x i8]* @.str.1, i64 0, i64 0), double noundef %div)
  %6 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8
  %call22 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* noundef %6, i8* noundef getelementptr inbounds ([12 x i8], [12 x i8]* @.str.2, i64 0, i64 0), i32 noundef %call10) #12
  br label %for.inc23

for.inc23:                                        ; preds = %for.body7
  %inc24 = add nuw nsw i32 %r.04, 1
  %cmp6 = icmp slt i32 %inc24, %call1
  br i1 %cmp6, label %for.body7, label %for.cond5.for.end25_crit_edge, !llvm.loop !9

for.cond5.for.end25_crit_edge:                    ; preds = %for.inc23
  br label %for.end25

for.end25:                                        ; preds = %for.cond5.for.end25_crit_edge, %for.end
  ret i32 0
}

declare dso_local i32 @fprintf(%struct._IO_FILE* noundef, i8* noundef, ...) #5

; Function Attrs: noreturn nounwind
declare dso_local void @exit(i32 noundef) #6

; Function Attrs: nounwind readonly willreturn
declare dso_local i32 @atoi(i8* noundef) #7

; Function Attrs: nounwind
declare dso_local noalias i8* @calloc(i64 noundef, i64 noundef) #8

; Function Attrs: nounwind
declare dso_local i64 @_ZNSt6chrono3_V212system_clock3nowEv() #8

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chrono13duration_castINS_8durationIlSt5ratioILl1ELl1000000EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) #1 comdat {
entry:
  %call = call i64 @_ZNSt6chrono20__duration_cast_implINS_8durationIlSt5ratioILl1ELl1000000EEEES2_ILl1ELl1000EElLb1ELb0EE6__castIlS2_ILl1ELl1000000000EEEES4_RKNS1_IT_T0_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d)
  ret i64 %call
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__lhs, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__rhs) #1 comdat {
entry:
  %ref.tmp = alloca %"struct.std::chrono::duration", align 8
  %ref.tmp1 = alloca %"struct.std::chrono::duration", align 8
  %call = call i64 @_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__lhs)
  %coerce.dive = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp, i64 0, i32 0
  store i64 %call, i64* %coerce.dive, align 8
  %call2 = call i64 @_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__rhs)
  %coerce.dive3 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp1, i64 0, i32 0
  store i64 %call2, i64* %coerce.dive3, align 8
  %call4 = call i64 @_ZNSt6chronomiIlSt5ratioILl1ELl1000000000EElS2_EENSt11common_typeIJNS_8durationIT_T0_EENS4_IT1_T2_EEEE4typeERKS7_RKSA_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp1)
  ret i64 %call4
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this) #0 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %0 = load i64, i64* %__r, align 8
  ret i64 %0
}

declare dso_local i32 @printf(i8* noundef, ...) #5

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chrono20__duration_cast_implINS_8durationIlSt5ratioILl1ELl1000000EEEES2_ILl1ELl1000EElLb1ELb0EE6__castIlS2_ILl1ELl1000000000EEEES4_RKNS1_IT_T0_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) #1 comdat align 2 {
entry:
  %retval = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp = alloca i64, align 8
  %call = call noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d)
  %div = sdiv i64 %call, 1000
  store i64 %div, i64* %ref.tmp, align 8
  call void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000EEEC2IlvEERKT_(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %retval, i64* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %coerce.dive = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %retval, i64 0, i32 0
  %0 = load i64, i64* %coerce.dive, align 8
  ret i64 %0
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this) #0 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__r, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000EEEC2IlvEERKT_(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this, i64* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #9 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %0 = load i64, i64* %__rep, align 8
  store i64 %0, i64* %__r, align 8
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chronomiIlSt5ratioILl1ELl1000000000EElS2_EENSt11common_typeIJNS_8durationIT_T0_EENS4_IT1_T2_EEEE4typeERKS7_RKSA_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__lhs, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__rhs) #1 comdat {
entry:
  %retval = alloca %"struct.std::chrono::duration", align 8
  %ref.tmp = alloca i64, align 8
  %ref.tmp1 = alloca %"struct.std::chrono::duration", align 8
  %ref.tmp2 = alloca %"struct.std::chrono::duration", align 8
  %0 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %__lhs, i64 0, i32 0
  %1 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp1, i64 0, i32 0
  %2 = load i64, i64* %0, align 8
  store i64 %2, i64* %1, align 8
  %call = call noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp1)
  %3 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %__rhs, i64 0, i32 0
  %4 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp2, i64 0, i32 0
  %5 = load i64, i64* %3, align 8
  store i64 %5, i64* %4, align 8
  %call3 = call noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp2)
  %sub = sub nsw i64 %call, %call3
  store i64 %sub, i64* %ref.tmp, align 8
  call void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %retval, i64* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %coerce.dive = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %retval, i64 0, i32 0
  %6 = load i64, i64* %coerce.dive, align 8
  ret i64 %6
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local i64 @_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %this) #0 comdat align 2 {
entry:
  %0 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %this, i64 0, i32 0, i32 0
  %1 = load i64, i64* %0, align 8
  ret i64 %1
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #10

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this, i64* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #9 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__rep, align 8
  store i64 %0, i64* %__r, align 8
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i64 @fwrite(i8* nocapture noundef, i64 noundef, i64 noundef, %struct._IO_FILE* nocapture noundef) #11

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { argmemonly nounwind willreturn }
attributes #3 = { argmemonly willreturn }
attributes #4 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { noreturn nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind readonly willreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { argmemonly nofree nounwind willreturn }
attributes #11 = { nofree nounwind }
attributes #12 = { cold }
attributes #13 = { noreturn nounwind }
attributes #14 = { nounwind readonly willreturn }
attributes #15 = { nounwind }

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
