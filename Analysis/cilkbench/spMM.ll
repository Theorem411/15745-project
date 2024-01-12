; ModuleID = 'spMM.bc'
source_filename = "spMM.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }
%"class.std::basic_ostream" = type { i32 (...)**, %"class.std::basic_ios" }
%"class.std::basic_ios" = type { %"class.std::ios_base", %"class.std::basic_ostream"*, i8, i8, %"class.std::basic_streambuf"*, %"class.std::ctype"*, %"class.std::num_put"*, %"class.std::num_get"* }
%"class.std::ios_base" = type { i32 (...)**, i64, i64, i32, i32, i32, %"struct.std::ios_base::_Callback_list"*, %"struct.std::ios_base::_Words", [8 x %"struct.std::ios_base::_Words"], i32, %"struct.std::ios_base::_Words"*, %"class.std::locale" }
%"struct.std::ios_base::_Callback_list" = type { %"struct.std::ios_base::_Callback_list"*, void (i32, %"class.std::ios_base"*, i32)*, i32, i32 }
%"struct.std::ios_base::_Words" = type { i8*, i64 }
%"class.std::locale" = type { %"class.std::locale::_Impl"* }
%"class.std::locale::_Impl" = type { i32, %"class.std::locale::facet"**, i64, %"class.std::locale::facet"**, i8** }
%"class.std::locale::facet" = type <{ i32 (...)**, i32, [4 x i8] }>
%"class.std::basic_streambuf" = type { i32 (...)**, i8*, i8*, i8*, i8*, i8*, i8*, %"class.std::locale" }
%"class.std::ctype" = type <{ %"class.std::locale::facet.base", [4 x i8], %struct.__locale_struct*, i8, [7 x i8], i32*, i32*, i16*, i8, [256 x i8], [256 x i8], i8, [6 x i8] }>
%"class.std::locale::facet.base" = type <{ i32 (...)**, i32 }>
%struct.__locale_struct = type { [13 x %struct.__locale_data*], i16*, i32*, i32*, [13 x i8*] }
%struct.__locale_data = type opaque
%"class.std::num_put" = type { %"class.std::locale::facet.base", [4 x i8] }
%"class.std::num_get" = type { %"class.std::locale::facet.base", [4 x i8] }
%struct.SparseMatrix = type { [201 x i32], [200 x i32], [200 x double] }
%"struct.std::chrono::time_point" = type { %"struct.std::chrono::duration" }
%"struct.std::chrono::duration" = type { i64 }
%"struct.std::chrono::duration.0" = type { double }

$_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE = comdat any

$_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IlS1_ILl1ELl1000000000EEvEERKNS0_IT_T0_EE = comdat any

$_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv = comdat any

$_ZNSt6chronomiIlSt5ratioILl1ELl1000000000EElS2_EENSt11common_typeIJNS_8durationIT_T0_EENS4_IT1_T2_EEEE4typeERKS7_RKSA_ = comdat any

$_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv = comdat any

$_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv = comdat any

$_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_ = comdat any

$_ZNSt6chrono13duration_castINS_8durationIdSt5ratioILl1ELl1EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE = comdat any

$_ZNSt6chrono20__duration_cast_implINS_8durationIdSt5ratioILl1ELl1EEEES2_ILl1ELl1000000000EEdLb1ELb0EE6__castIlS5_EES4_RKNS1_IT_T0_EE = comdat any

$_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IdvEERKT_ = comdat any

@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@_ZSt4cout = external dso_local global %"class.std::basic_ostream", align 8
@.str = private unnamed_addr constant [43 x i8] c"Sparse Matrix-Matrix Multiplication Time: \00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c" seconds\0A\00", align 1
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_spMM.cpp, i8* null }]

; Function Attrs: noinline uwtable
define internal void @__cxx_global_var_init() #0 section ".text.startup" {
entry:
  call void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"* noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %0 = call i32 @__cxa_atexit(void (i8*)* bitcast (void (%"class.std::ios_base::Init"*)* @_ZNSt8ios_base4InitD1Ev to void (i8*)*), i8* getelementptr inbounds (%"class.std::ios_base::Init", %"class.std::ios_base::Init"* @_ZStL8__ioinit, i64 0, i32 0), i8* nonnull @__dso_handle) #3
  ret void
}

declare dso_local void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"* noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nounwind
declare dso_local void @_ZNSt8ios_base4InitD1Ev(%"class.std::ios_base::Init"* noundef nonnull align 1 dereferenceable(1)) unnamed_addr #2

; Function Attrs: nounwind
declare dso_local i32 @__cxa_atexit(void (i8*)*, i8*, i8*) #3

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z26sparseMatrixMatrixMultiplyRK12SparseMatrixS1_PA200_d(%struct.SparseMatrix* noundef nonnull align 8 dereferenceable(3208) %matrixA, %struct.SparseMatrix* noundef nonnull align 8 dereferenceable(3208) %matrixB, [200 x double]* noundef %result) #4 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  br label %for.body

for.body:                                         ; preds = %entry, %for.inc6
  %i.02 = phi i32 [ 0, %entry ], [ %inc7, %for.inc6 ]
  br label %for.body3

for.body3:                                        ; preds = %for.body, %for.inc
  %j.01 = phi i32 [ 0, %for.body ], [ %inc, %for.inc ]
  %idxprom = zext i32 %i.02 to i64
  %idxprom4 = zext i32 %j.01 to i64
  %arrayidx5 = getelementptr inbounds [200 x double], [200 x double]* %result, i64 %idxprom, i64 %idxprom4
  store double 0.000000e+00, double* %arrayidx5, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body3
  %inc = add nuw nsw i32 %j.01, 1
  %cmp2 = icmp ult i32 %j.01, 199
  br i1 %cmp2, label %for.body3, label %for.end, !llvm.loop !4

for.end:                                          ; preds = %for.inc
  br label %for.inc6

for.inc6:                                         ; preds = %for.end
  %inc7 = add nuw nsw i32 %i.02, 1
  %cmp = icmp ult i32 %i.02, 199
  br i1 %cmp, label %for.body, label %for.end8, !llvm.loop !6

for.end8:                                         ; preds = %for.inc6
  br i1 true, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %for.end8
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc42, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  br label %for.body16

for.body16:                                       ; preds = %pfor.body, %for.inc39
  %j13.05 = phi i32 [ 0, %pfor.body ], [ %inc40, %for.inc39 ]
  %idxprom17 = zext i32 %j13.05 to i64
  %arrayidx18 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixB, i64 0, i32 0, i64 %idxprom17
  %0 = load i32, i32* %arrayidx18, align 4
  %add21 = add nuw nsw i32 %j13.05, 1
  %idxprom22 = zext i32 %add21 to i64
  %arrayidx23 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixB, i64 0, i32 0, i64 %idxprom22
  %1 = load i32, i32* %arrayidx23, align 4
  %cmp243 = icmp slt i32 %0, %1
  br i1 %cmp243, label %for.body25.lr.ph, label %for.end38

for.body25.lr.ph:                                 ; preds = %for.body16
  br label %for.body25

for.body25:                                       ; preds = %for.body25.lr.ph, %for.inc36
  %k.04 = phi i32 [ %0, %for.body25.lr.ph ], [ %inc37, %for.inc36 ]
  %idxprom26 = zext i32 %__begin.0 to i64
  %arrayidx27 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixA, i64 0, i32 2, i64 %idxprom26
  %2 = load double, double* %arrayidx27, align 8
  %idxprom29 = sext i32 %k.04 to i64
  %arrayidx30 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixB, i64 0, i32 2, i64 %idxprom29
  %3 = load double, double* %arrayidx30, align 8
  %idxprom32 = zext i32 %__begin.0 to i64
  %idxprom34 = zext i32 %j13.05 to i64
  %arrayidx35 = getelementptr inbounds [200 x double], [200 x double]* %result, i64 %idxprom32, i64 %idxprom34
  %4 = load double, double* %arrayidx35, align 8
  %5 = call double @llvm.fmuladd.f64(double %2, double %3, double %4)
  store double %5, double* %arrayidx35, align 8
  br label %for.inc36

for.inc36:                                        ; preds = %for.body25
  %inc37 = add nsw i32 %k.04, 1
  %6 = load i32, i32* %arrayidx23, align 4
  %cmp24 = icmp slt i32 %inc37, %6
  br i1 %cmp24, label %for.body25, label %for.cond19.for.end38_crit_edge, !llvm.loop !7

for.cond19.for.end38_crit_edge:                   ; preds = %for.inc36
  br label %for.end38

for.end38:                                        ; preds = %for.cond19.for.end38_crit_edge, %for.body16
  br label %for.inc39

for.inc39:                                        ; preds = %for.end38
  %inc40 = add nuw nsw i32 %j13.05, 1
  %cmp15 = icmp ult i32 %j13.05, 199
  br i1 %cmp15, label %for.body16, label %for.end41, !llvm.loop !8

for.end41:                                        ; preds = %for.inc39
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %for.end41
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc42 = add nuw nsw i32 %__begin.0, 1
  %cmp43 = icmp ult i32 %__begin.0, 199
  br i1 %cmp43, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !9

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %for.end8
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #5

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.fmuladd.f64(double, double, double) #6

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #7

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main() #8 {
entry:
  %matrixA = alloca %struct.SparseMatrix, align 8
  %matrixB = alloca %struct.SparseMatrix, align 8
  %result = alloca [200 x [200 x double]], align 16
  %start = alloca %"struct.std::chrono::time_point", align 8
  %end = alloca %"struct.std::chrono::time_point", align 8
  %duration = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp = alloca %"struct.std::chrono::duration", align 8
  br label %for.body

for.body:                                         ; preds = %entry, %for.inc
  %i.01 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]
  %idxprom = zext i32 %i.01 to i64
  %arrayidx = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixA, i64 0, i32 0, i64 %idxprom
  store i32 %i.01, i32* %arrayidx, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %i.01, 1
  %cmp = icmp ult i32 %i.01, 200
  br i1 %cmp, label %for.body, label %for.end, !llvm.loop !11

for.end:                                          ; preds = %for.inc
  br label %for.body4

for.body4:                                        ; preds = %for.end, %for.inc9
  %i1.02 = phi i32 [ 0, %for.end ], [ %inc10, %for.inc9 ]
  %rem = urem i32 %i1.02, 200
  %idxprom5 = zext i32 %i1.02 to i64
  %arrayidx6 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixA, i64 0, i32 1, i64 %idxprom5
  store i32 %rem, i32* %arrayidx6, align 4
  %add = add nuw nsw i32 %i1.02, 1
  %conv = sitofp i32 %add to double
  %idxprom7 = zext i32 %i1.02 to i64
  %arrayidx8 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixA, i64 0, i32 2, i64 %idxprom7
  store double %conv, double* %arrayidx8, align 8
  br label %for.inc9

for.inc9:                                         ; preds = %for.body4
  %inc10 = add nuw nsw i32 %i1.02, 1
  %cmp3 = icmp ult i32 %i1.02, 199
  br i1 %cmp3, label %for.body4, label %for.end11, !llvm.loop !12

for.end11:                                        ; preds = %for.inc9
  br label %for.body15

for.body15:                                       ; preds = %for.end11, %for.inc20
  %i12.03 = phi i32 [ 0, %for.end11 ], [ %inc21, %for.inc20 ]
  %idxprom18 = zext i32 %i12.03 to i64
  %arrayidx19 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixB, i64 0, i32 0, i64 %idxprom18
  store i32 %i12.03, i32* %arrayidx19, align 4
  br label %for.inc20

for.inc20:                                        ; preds = %for.body15
  %inc21 = add nuw nsw i32 %i12.03, 1
  %cmp14 = icmp ult i32 %i12.03, 200
  br i1 %cmp14, label %for.body15, label %for.end22, !llvm.loop !13

for.end22:                                        ; preds = %for.inc20
  br label %for.body26

for.body26:                                       ; preds = %for.end22, %for.inc36
  %i23.04 = phi i32 [ 0, %for.end22 ], [ %inc37, %for.inc36 ]
  %rem27 = urem i32 %i23.04, 200
  %idxprom29 = zext i32 %i23.04 to i64
  %arrayidx30 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixB, i64 0, i32 1, i64 %idxprom29
  store i32 %rem27, i32* %arrayidx30, align 4
  %add31 = add nuw nsw i32 %i23.04, 1
  %conv32 = sitofp i32 %add31 to double
  %idxprom34 = zext i32 %i23.04 to i64
  %arrayidx35 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrixB, i64 0, i32 2, i64 %idxprom34
  store double %conv32, double* %arrayidx35, align 8
  br label %for.inc36

for.inc36:                                        ; preds = %for.body26
  %inc37 = add nuw nsw i32 %i23.04, 1
  %cmp25 = icmp ult i32 %i23.04, 199
  br i1 %cmp25, label %for.body26, label %for.end38, !llvm.loop !14

for.end38:                                        ; preds = %for.inc36
  %call = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive39 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %start, i64 0, i32 0, i32 0
  store i64 %call, i64* %coerce.dive39, align 8
  %arraydecay = getelementptr inbounds [200 x [200 x double]], [200 x [200 x double]]* %result, i64 0, i64 0
  call void @_Z26sparseMatrixMatrixMultiplyRK12SparseMatrixS1_PA200_d(%struct.SparseMatrix* noundef nonnull align 8 dereferenceable(3208) %matrixA, %struct.SparseMatrix* noundef nonnull align 8 dereferenceable(3208) %matrixB, [200 x double]* noundef nonnull %arraydecay)
  %call40 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive42 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %end, i64 0, i32 0, i32 0
  store i64 %call40, i64* %coerce.dive42, align 8
  %call43 = call i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %end, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %start)
  %coerce.dive44 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp, i64 0, i32 0
  store i64 %call43, i64* %coerce.dive44, align 8
  call void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IlS1_ILl1ELl1000000000EEvEERKNS0_IT_T0_EE(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %call45 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i8* noundef getelementptr inbounds ([43 x i8], [43 x i8]* @.str, i64 0, i64 0))
  %call46 = call noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration)
  %call47 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call45, double noundef %call46)
  %call48 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call47, i8* noundef getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i64 0, i64 0))
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i64 @_ZNSt6chrono3_V212system_clock3nowEv() #2

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__lhs, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__rhs) #4 comdat {
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

; Function Attrs: noinline uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IlS1_ILl1ELl1000000000EEvEERKNS0_IT_T0_EE(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) unnamed_addr #0 comdat align 2 {
entry:
  %ref.tmp = alloca %"struct.std::chrono::duration.0", align 8
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %call = call double @_ZNSt6chrono13duration_castINS_8durationIdSt5ratioILl1ELl1EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d)
  %coerce.dive = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %ref.tmp, i64 0, i32 0
  store double %call, double* %coerce.dive, align 8
  %call2 = call noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  store double %call2, double* %__r, align 8
  ret void
}

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), i8* noundef) #1

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), double noundef) #1

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this) #9 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %0 = load double, double* %__r, align 8
  ret double %0
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chronomiIlSt5ratioILl1ELl1000000000EElS2_EENSt11common_typeIJNS_8durationIT_T0_EENS4_IT1_T2_EEEE4typeERKS7_RKSA_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__lhs, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__rhs) #4 comdat {
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
define linkonce_odr dso_local i64 @_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %this) #9 comdat align 2 {
entry:
  %0 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %this, i64 0, i32 0, i32 0
  %1 = load i64, i64* %0, align 8
  ret i64 %1
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #10

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this) #9 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__r, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this, i64* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #11 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__rep, align 8
  store i64 %0, i64* %__r, align 8
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local double @_ZNSt6chrono13duration_castINS_8durationIdSt5ratioILl1ELl1EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) #4 comdat {
entry:
  %call = call double @_ZNSt6chrono20__duration_cast_implINS_8durationIdSt5ratioILl1ELl1EEEES2_ILl1ELl1000000000EEdLb1ELb0EE6__castIlS5_EES4_RKNS1_IT_T0_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d)
  ret double %call
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local double @_ZNSt6chrono20__duration_cast_implINS_8durationIdSt5ratioILl1ELl1EEEES2_ILl1ELl1000000000EEdLb1ELb0EE6__castIlS5_EES4_RKNS1_IT_T0_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) #4 comdat align 2 {
entry:
  %retval = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp = alloca double, align 8
  %call = call noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d)
  %conv = sitofp i64 %call to double
  %div = fdiv double %conv, 1.000000e+09
  store double %div, double* %ref.tmp, align 8
  call void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IdvEERKT_(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %retval, double* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %coerce.dive = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %retval, i64 0, i32 0
  %0 = load double, double* %coerce.dive, align 8
  ret double %0
}

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IdvEERKT_(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this, double* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #11 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %0 = load double, double* %__rep, align 8
  store double %0, double* %__r, align 8
  ret void
}

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_spMM.cpp() #0 section ".text.startup" {
entry:
  call void @__cxx_global_var_init()
  ret void
}

attributes #0 = { noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }
attributes #4 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { argmemonly nounwind willreturn }
attributes #6 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #7 = { argmemonly willreturn }
attributes #8 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { argmemonly nofree nounwind willreturn }
attributes #11 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !5}
!7 = distinct !{!7, !5}
!8 = distinct !{!8, !5}
!9 = distinct !{!9, !10}
!10 = !{!"tapir.loop.spawn.strategy", i32 1}
!11 = distinct !{!11, !5}
!12 = distinct !{!12, !5}
!13 = distinct !{!13, !5}
!14 = distinct !{!14, !5}
