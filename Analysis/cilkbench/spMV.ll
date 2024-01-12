; ModuleID = 'spMV.bc'
source_filename = "spMV.cpp"
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
%struct.SparseMatrix = type { [1001 x i32], [5000 x i32], [5000 x double] }
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
@__const.main.matrix = private unnamed_addr constant { <{ [11 x i32], [990 x i32] }>, <{ [20 x i32], [4980 x i32] }>, [4 x i8], <{ [20 x double], [4980 x double] }> } { <{ [11 x i32], [990 x i32] }> <{ [11 x i32] [i32 0, i32 2, i32 4, i32 7, i32 9, i32 10, i32 12, i32 14, i32 16, i32 19, i32 20], [990 x i32] zeroinitializer }>, <{ [20 x i32], [4980 x i32] }> <{ [20 x i32] [i32 0, i32 1, i32 1, i32 2, i32 3, i32 0, i32 2, i32 4, i32 0, i32 1, i32 3, i32 4, i32 0, i32 3, i32 4, i32 1, i32 2, i32 4, i32 3, i32 4], [4980 x i32] zeroinitializer }>, [4 x i8] undef, <{ [20 x double], [4980 x double] }> <{ [20 x double] [double 1.000000e+00, double 2.000000e+00, double 3.000000e+00, double 4.000000e+00, double 5.000000e+00, double 6.000000e+00, double 7.000000e+00, double 8.000000e+00, double 9.000000e+00, double 1.000000e+01, double 1.100000e+01, double 1.200000e+01, double 1.300000e+01, double 1.400000e+01, double 1.500000e+01, double 1.600000e+01, double 1.700000e+01, double 1.800000e+01, double 1.900000e+01, double 2.000000e+01], [4980 x double] zeroinitializer }> }, align 8
@_ZSt4cout = external dso_local global %"class.std::basic_ostream", align 8
@.str = private unnamed_addr constant [43 x i8] c"Sparse Matrix-Vector Multiplication Time: \00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c" seconds\0A\00", align 1
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_spMV.cpp, i8* null }]

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
define dso_local void @_Z26sparseMatrixVectorMultiplyRK12SparseMatrixPKdPd(%struct.SparseMatrix* noundef nonnull align 8 dereferenceable(64008) %matrix, double* noundef %vector, double* noundef %result) #4 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  br i1 true, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %entry
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc19, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %idxprom = zext i32 %__begin.0 to i64
  %arrayidx = getelementptr inbounds double, double* %result, i64 %idxprom
  store double 0.000000e+00, double* %arrayidx, align 8
  %idxprom3 = zext i32 %__begin.0 to i64
  %arrayidx4 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrix, i64 0, i32 0, i64 %idxprom3
  %0 = load i32, i32* %arrayidx4, align 4
  %add6 = add nuw nsw i32 %__begin.0, 1
  %idxprom7 = zext i32 %add6 to i64
  %arrayidx8 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrix, i64 0, i32 0, i64 %idxprom7
  %1 = load i32, i32* %arrayidx8, align 4
  %cmp91 = icmp slt i32 %0, %1
  br i1 %cmp91, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %pfor.body
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %j.02 = phi i32 [ %0, %for.body.lr.ph ], [ %inc, %for.inc ]
  %idxprom10 = sext i32 %j.02 to i64
  %arrayidx11 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrix, i64 0, i32 2, i64 %idxprom10
  %2 = load double, double* %arrayidx11, align 8
  %idxprom12 = sext i32 %j.02 to i64
  %arrayidx13 = getelementptr inbounds %struct.SparseMatrix, %struct.SparseMatrix* %matrix, i64 0, i32 1, i64 %idxprom12
  %3 = load i32, i32* %arrayidx13, align 4
  %idxprom14 = sext i32 %3 to i64
  %arrayidx15 = getelementptr inbounds double, double* %vector, i64 %idxprom14
  %4 = load double, double* %arrayidx15, align 8
  %idxprom17 = zext i32 %__begin.0 to i64
  %arrayidx18 = getelementptr inbounds double, double* %result, i64 %idxprom17
  %5 = load double, double* %arrayidx18, align 8
  %6 = call double @llvm.fmuladd.f64(double %2, double %4, double %5)
  store double %6, double* %arrayidx18, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nsw i32 %j.02, 1
  %7 = load i32, i32* %arrayidx8, align 4
  %cmp9 = icmp slt i32 %inc, %7
  br i1 %cmp9, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !4

for.cond.for.end_crit_edge:                       ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %pfor.body
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %for.end
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc19 = add nuw nsw i32 %__begin.0, 1
  %cmp20 = icmp ult i32 %__begin.0, 999
  br i1 %cmp20, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !6

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %entry
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
  %matrix = alloca %struct.SparseMatrix, align 8
  %vector = alloca [1000 x double], align 16
  %result = alloca [1000 x double], align 16
  %start = alloca %"struct.std::chrono::time_point", align 8
  %end = alloca %"struct.std::chrono::time_point", align 8
  %duration = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp = alloca %"struct.std::chrono::duration", align 8
  %0 = bitcast %struct.SparseMatrix* %matrix to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* noundef nonnull align 8 dereferenceable(64008) %0, i8* noundef nonnull align 8 dereferenceable(64008) bitcast ({ <{ [11 x i32], [990 x i32] }>, <{ [20 x i32], [4980 x i32] }>, [4 x i8], <{ [20 x double], [4980 x double] }> }* @__const.main.matrix to i8*), i64 64008, i1 false)
  %call = call i64 @time(i64* noundef null) #3
  %conv = trunc i64 %call to i32
  call void @srand(i32 noundef %conv) #3
  br label %for.body

for.body:                                         ; preds = %entry, %for.inc
  %i.01 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]
  %call1 = call i32 @rand() #3
  %conv2 = sitofp i32 %call1 to double
  %div = fdiv double %conv2, 0x41DFFFFFFFC00000
  %idxprom = zext i32 %i.01 to i64
  %arrayidx = getelementptr inbounds [1000 x double], [1000 x double]* %vector, i64 0, i64 %idxprom
  store double %div, double* %arrayidx, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %i.01, 1
  %cmp = icmp ult i32 %i.01, 999
  br i1 %cmp, label %for.body, label %for.end, !llvm.loop !8

for.end:                                          ; preds = %for.inc
  %call3 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive4 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %start, i64 0, i32 0, i32 0
  store i64 %call3, i64* %coerce.dive4, align 8
  %arraydecay = getelementptr inbounds [1000 x double], [1000 x double]* %vector, i64 0, i64 0
  %arraydecay5 = getelementptr inbounds [1000 x double], [1000 x double]* %result, i64 0, i64 0
  call void @_Z26sparseMatrixVectorMultiplyRK12SparseMatrixPKdPd(%struct.SparseMatrix* noundef nonnull align 8 dereferenceable(64008) %matrix, double* noundef nonnull %arraydecay, double* noundef nonnull %arraydecay5)
  %call6 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive8 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %end, i64 0, i32 0, i32 0
  store i64 %call6, i64* %coerce.dive8, align 8
  %call9 = call i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %end, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %start)
  %coerce.dive10 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp, i64 0, i32 0
  store i64 %call9, i64* %coerce.dive10, align 8
  call void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IlS1_ILl1ELl1000000000EEvEERKNS0_IT_T0_EE(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %call11 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i8* noundef getelementptr inbounds ([43 x i8], [43 x i8]* @.str, i64 0, i64 0))
  %call12 = call noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration)
  %call13 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call11, double noundef %call12)
  %call14 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call13, i8* noundef getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i64 0, i64 0))
  ret i32 0
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #9

; Function Attrs: nounwind
declare dso_local void @srand(i32 noundef) #2

; Function Attrs: nounwind
declare dso_local i64 @time(i64* noundef) #2

; Function Attrs: nounwind
declare dso_local i32 @rand() #2

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
define linkonce_odr dso_local noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this) #10 comdat align 2 {
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
define linkonce_odr dso_local i64 @_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %this) #10 comdat align 2 {
entry:
  %0 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %this, i64 0, i32 0, i32 0
  %1 = load i64, i64* %0, align 8
  ret i64 %1
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this) #10 comdat align 2 {
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
define internal void @_GLOBAL__sub_I_spMV.cpp() #0 section ".text.startup" {
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
attributes #9 = { argmemonly nofree nounwind willreturn }
attributes #10 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !7}
!7 = !{!"tapir.loop.spawn.strategy", i32 1}
!8 = distinct !{!8, !5}
