; ModuleID = 'quicksort.bc'
source_filename = "quicksort.cpp"
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
%"struct.std::chrono::time_point" = type { %"struct.std::chrono::duration" }
%"struct.std::chrono::duration" = type { i64 }
%"struct.std::chrono::duration.0" = type { double }

$_ZSt4swapIiEvRT_S1_ = comdat any

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

$_ZSt4moveIRiEONSt16remove_referenceIT_E4typeEOS2_ = comdat any

@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@_ZSt4cout = external dso_local global %"class.std::basic_ostream", align 8
@.str = private unnamed_addr constant [2 x i8] c" \00", align 1
@__const.main.arr = private unnamed_addr constant [8 x i32] [i32 12, i32 4, i32 5, i32 6, i32 7, i32 3, i32 1, i32 15], align 16
@.str.1 = private unnamed_addr constant [17 x i8] c"Quicksort Time: \00", align 1
@.str.2 = private unnamed_addr constant [10 x i8] c" seconds\0A\00", align 1
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_quicksort.cpp, i8* null }]

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
define dso_local void @_Z9quicksortPiii(i32* noundef %arr, i32 noundef %low, i32 noundef %high) #4 {
entry:
  %i = alloca i32, align 4
  %syncreg = call token @llvm.syncregion.start()
  %syncreg20 = call token @llvm.syncregion.start()
  %cmp = icmp slt i32 %low, %high
  br i1 %cmp, label %if.then, label %if.end24

if.then:                                          ; preds = %entry
  %idxprom = sext i32 %high to i64
  %arrayidx = getelementptr inbounds i32, i32* %arr, i64 %idxprom
  %0 = load i32, i32* %arrayidx, align 4
  %sub = add nsw i32 %low, -1
  store i32 %sub, i32* %i, align 4
  br i1 true, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %if.then
  %1 = xor i32 %low, -1
  %sub3 = add i32 %1, %high
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc12, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  %add = add nsw i32 %__begin.0, %low
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %idxprom4 = sext i32 %add to i64
  %arrayidx5 = getelementptr inbounds i32, i32* %arr, i64 %idxprom4
  %2 = load i32, i32* %arrayidx5, align 4
  %cmp6 = icmp slt i32 %2, %0
  br i1 %cmp6, label %if.then7, label %if.end

if.then7:                                         ; preds = %pfor.body
  %3 = load i32, i32* %i, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, i32* %i, align 4
  %idxprom8 = sext i32 %inc to i64
  %arrayidx9 = getelementptr inbounds i32, i32* %arr, i64 %idxprom8
  %idxprom10 = sext i32 %add to i64
  %arrayidx11 = getelementptr inbounds i32, i32* %arr, i64 %idxprom10
  call void @_ZSt4swapIiEvRT_S1_(i32* noundef nonnull align 4 dereferenceable(4) %arrayidx9, i32* noundef nonnull align 4 dereferenceable(4) %arrayidx11) #3
  br label %if.end

if.end:                                           ; preds = %if.then7, %pfor.body
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %if.end
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc12 = add nuw nsw i32 %__begin.0, 1
  %cmp13.not.not = icmp slt i32 %__begin.0, %sub3
  br i1 %cmp13.not.not, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %if.then
  %4 = load i32, i32* %i, align 4
  %add14 = add nsw i32 %4, 1
  %idxprom15 = sext i32 %add14 to i64
  %arrayidx16 = getelementptr inbounds i32, i32* %arr, i64 %idxprom15
  %idxprom17 = sext i32 %high to i64
  %arrayidx18 = getelementptr inbounds i32, i32* %arr, i64 %idxprom17
  call void @_ZSt4swapIiEvRT_S1_(i32* noundef nonnull align 4 dereferenceable(4) %arrayidx16, i32* noundef nonnull align 4 dereferenceable(4) %arrayidx18) #3
  %5 = load i32, i32* %i, align 4
  br label %pfor.end.tf

pfor.end.tf:                                      ; preds = %pfor.end
  %6 = call token @llvm.taskframe.create()
  detach within %syncreg20, label %det.achd, label %det.cont

det.achd:                                         ; preds = %pfor.end.tf
  call void @llvm.taskframe.use(token %6)
  call void @_Z9quicksortPiii(i32* noundef %arr, i32 noundef %low, i32 noundef %5)
  reattach within %syncreg20, label %det.cont

det.cont:                                         ; preds = %det.achd, %pfor.end.tf
  %add22 = add nsw i32 %5, 2
  call void @_Z9quicksortPiii(i32* noundef %arr, i32 noundef %add22, i32 noundef %high)
  sync within %syncreg20, label %sync.continue23

sync.continue23:                                  ; preds = %det.cont
  call void @llvm.sync.unwind(token %syncreg20)
  br label %if.end24

if.end24:                                         ; preds = %sync.continue23, %entry
  sync within %syncreg20, label %sync.continue25

sync.continue25:                                  ; preds = %if.end24
  call void @llvm.sync.unwind(token %syncreg20)
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #5

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local void @_ZSt4swapIiEvRT_S1_(i32* noundef nonnull align 4 dereferenceable(4) %__a, i32* noundef nonnull align 4 dereferenceable(4) %__b) #6 comdat {
entry:
  %__tmp = alloca i32, align 4
  %call = call noundef nonnull align 4 dereferenceable(4) i32* @_ZSt4moveIRiEONSt16remove_referenceIT_E4typeEOS2_(i32* noundef nonnull align 4 dereferenceable(4) %__a) #3
  %0 = load i32, i32* %call, align 4
  store i32 %0, i32* %__tmp, align 4
  %call1 = call noundef nonnull align 4 dereferenceable(4) i32* @_ZSt4moveIRiEONSt16remove_referenceIT_E4typeEOS2_(i32* noundef nonnull align 4 dereferenceable(4) %__b) #3
  %1 = load i32, i32* %call1, align 4
  store i32 %1, i32* %__a, align 4
  %call2 = call noundef nonnull align 4 dereferenceable(4) i32* @_ZSt4moveIRiEONSt16remove_referenceIT_E4typeEOS2_(i32* noundef nonnull align 4 dereferenceable(4) %__tmp) #3
  %2 = load i32, i32* %call2, align 4
  store i32 %2, i32* %__b, align 4
  ret void
}

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #7

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.taskframe.create() #5

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.taskframe.use(token) #5

; Function Attrs: mustprogress noinline uwtable
define dso_local void @_Z10printArrayPii(i32* noundef %arr, i32 noundef %size) #4 {
entry:
  %cmp1 = icmp sgt i32 %size, 0
  br i1 %cmp1, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %entry
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %i.02 = phi i32 [ 0, %for.body.lr.ph ], [ %inc, %for.inc ]
  %idxprom = zext i32 %i.02 to i64
  %arrayidx = getelementptr inbounds i32, i32* %arr, i64 %idxprom
  %0 = load i32, i32* %arrayidx, align 4
  %call = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i32 noundef %0)
  %call1 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call, i8* noundef getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0))
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %i.02, 1
  %cmp = icmp slt i32 %inc, %size
  br i1 %cmp, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !6

for.cond.for.end_crit_edge:                       ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %entry
  %call2 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEPFRSoS_E(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, %"class.std::basic_ostream"* (%"class.std::basic_ostream"*)* noundef nonnull @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
  ret void
}

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), i8* noundef) #1

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), i32 noundef) #1

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEPFRSoS_E(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), %"class.std::basic_ostream"* (%"class.std::basic_ostream"*)* noundef) #1

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8)) #1

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main() #8 {
entry:
  %arr = alloca [8 x i32], align 16
  %start = alloca %"struct.std::chrono::time_point", align 8
  %end = alloca %"struct.std::chrono::time_point", align 8
  %duration = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp = alloca %"struct.std::chrono::duration", align 8
  %0 = bitcast [8 x i32]* %arr to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* noundef nonnull align 16 dereferenceable(32) %0, i8* noundef nonnull align 16 dereferenceable(32) bitcast ([8 x i32]* @__const.main.arr to i8*), i64 32, i1 false)
  %call = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive1 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %start, i64 0, i32 0, i32 0
  store i64 %call, i64* %coerce.dive1, align 8
  %arraydecay = getelementptr inbounds [8 x i32], [8 x i32]* %arr, i64 0, i64 0
  call void @_Z9quicksortPiii(i32* noundef nonnull %arraydecay, i32 noundef 0, i32 noundef 7)
  %call2 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive4 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %end, i64 0, i32 0, i32 0
  store i64 %call2, i64* %coerce.dive4, align 8
  %call5 = call i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %end, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %start)
  %coerce.dive6 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp, i64 0, i32 0
  store i64 %call5, i64* %coerce.dive6, align 8
  call void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IlS1_ILl1ELl1000000000EEvEERKNS0_IT_T0_EE(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %call7 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i8* noundef getelementptr inbounds ([17 x i8], [17 x i8]* @.str.1, i64 0, i64 0))
  %call8 = call noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration)
  %call9 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call7, double noundef %call8)
  %call10 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call9, i8* noundef getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i64 0, i64 0))
  ret i32 0
}

; Function Attrs: argmemonly nofree nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #9

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

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), double noundef) #1

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this) #6 comdat align 2 {
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
define linkonce_odr dso_local i64 @_ZNKSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEE16time_since_epochEv(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %this) #6 comdat align 2 {
entry:
  %0 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %this, i64 0, i32 0, i32 0
  %1 = load i64, i64* %0, align 8
  ret i64 %1
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this) #6 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__r, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this, i64* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #10 comdat align 2 {
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
define linkonce_odr dso_local void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IdvEERKT_(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this, double* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #10 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %0 = load double, double* %__rep, align 8
  store double %0, double* %__r, align 8
  ret void
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef nonnull align 4 dereferenceable(4) i32* @_ZSt4moveIRiEONSt16remove_referenceIT_E4typeEOS2_(i32* noundef nonnull align 4 dereferenceable(4) %__t) #6 comdat {
entry:
  ret i32* %__t
}

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_quicksort.cpp() #0 section ".text.startup" {
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
attributes #6 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { argmemonly willreturn }
attributes #8 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { argmemonly nofree nounwind willreturn }
attributes #10 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
