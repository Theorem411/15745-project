; ModuleID = 'cilkfor-doublenest.bc'
source_filename = "cilkfor-doublenest.cpp"
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

$_Z7readTSCv = comdat any

@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@_ZSt4cout = external dso_local global %"class.std::basic_ostream", align 8
@.str = private unnamed_addr constant [15 x i8] c"clock cycles: \00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_cilkfor_doublenest.cpp, i8* null }]

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
define dso_local noundef i32 @_Z4loopPA10_A10_ii([10 x [10 x i32]]* noundef %A, i32 noundef %N) #4 {
entry:
  %syncreg = call token @llvm.syncregion.start()
  %call = call noundef i64 @_Z7readTSCv()
  %cmp = icmp sgt i32 %N, 0
  br i1 %cmp, label %pfor.ph, label %pfor.end55

pfor.ph:                                          ; preds = %entry
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc50, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc51, %pfor.inc50 ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc50

pfor.body.entry:                                  ; preds = %pfor.detach
  %syncreg3 = call token @llvm.syncregion.start()
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %cmp6 = icmp sgt i32 %N, 1
  br i1 %cmp6, label %pfor.ph7, label %pfor.end48

pfor.ph7:                                         ; preds = %pfor.body
  %add13 = add nsw i32 %N, -1
  br label %pfor.cond14

pfor.cond14:                                      ; preds = %pfor.inc43, %pfor.ph7
  %__begin8.0 = phi i32 [ 0, %pfor.ph7 ], [ %inc44, %pfor.inc43 ]
  br label %pfor.detach15

pfor.detach15:                                    ; preds = %pfor.cond14
  %add17 = add nuw nsw i32 %__begin8.0, 1
  detach within %syncreg3, label %pfor.body.entry18, label %pfor.inc43

pfor.body.entry18:                                ; preds = %pfor.detach15
  %syncreg20 = call token @llvm.syncregion.start()
  br label %pfor.body19

pfor.body19:                                      ; preds = %pfor.body.entry18
  %cmp23 = icmp sgt i32 %N, 2
  br i1 %cmp23, label %pfor.ph24, label %pfor.end

pfor.ph24:                                        ; preds = %pfor.body19
  %add30 = add nsw i32 %N, -2
  br label %pfor.cond31

pfor.cond31:                                      ; preds = %pfor.inc, %pfor.ph24
  %__begin25.0 = phi i32 [ 0, %pfor.ph24 ], [ %inc, %pfor.inc ]
  br label %pfor.detach32

pfor.detach32:                                    ; preds = %pfor.cond31
  %add34 = add nuw nsw i32 %__begin25.0, 2
  detach within %syncreg20, label %pfor.body.entry35, label %pfor.inc

pfor.body.entry35:                                ; preds = %pfor.detach32
  br label %pfor.body36

pfor.body36:                                      ; preds = %pfor.body.entry35
  %idxprom = zext i32 %__begin.0 to i64
  %idxprom37 = zext i32 %add17 to i64
  %idxprom39 = zext i32 %add34 to i64
  %arrayidx40 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* %A, i64 %idxprom, i64 %idxprom37, i64 %idxprom39
  store i32 1, i32* %arrayidx40, align 4
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %pfor.body36
  reattach within %syncreg20, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach32
  %inc = add nuw nsw i32 %__begin25.0, 1
  %cmp41 = icmp slt i32 %inc, %add30
  br i1 %cmp41, label %pfor.cond31, label %pfor.cond.cleanup, !llvm.loop !4

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg20, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg20)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %pfor.body19
  br label %pfor.preattach42

pfor.preattach42:                                 ; preds = %pfor.end
  reattach within %syncreg3, label %pfor.inc43

pfor.inc43:                                       ; preds = %pfor.preattach42, %pfor.detach15
  %inc44 = add nuw nsw i32 %__begin8.0, 1
  %cmp45 = icmp slt i32 %inc44, %add13
  br i1 %cmp45, label %pfor.cond14, label %pfor.cond.cleanup46, !llvm.loop !6

pfor.cond.cleanup46:                              ; preds = %pfor.inc43
  sync within %syncreg3, label %sync.continue47

sync.continue47:                                  ; preds = %pfor.cond.cleanup46
  call void @llvm.sync.unwind(token %syncreg3)
  br label %pfor.end48

pfor.end48:                                       ; preds = %sync.continue47, %pfor.body
  br label %pfor.preattach49

pfor.preattach49:                                 ; preds = %pfor.end48
  reattach within %syncreg, label %pfor.inc50

pfor.inc50:                                       ; preds = %pfor.preattach49, %pfor.detach
  %inc51 = add nuw nsw i32 %__begin.0, 1
  %cmp52 = icmp slt i32 %inc51, %N
  br i1 %cmp52, label %pfor.cond, label %pfor.cond.cleanup53, !llvm.loop !7

pfor.cond.cleanup53:                              ; preds = %pfor.inc50
  sync within %syncreg, label %sync.continue54

sync.continue54:                                  ; preds = %pfor.cond.cleanup53
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end55

pfor.end55:                                       ; preds = %sync.continue54, %entry
  %call56 = call noundef i64 @_Z7readTSCv()
  %call57 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i8* noundef getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i64 0, i64 0))
  %sub58 = sub i64 %call56, %call
  %call59 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEy(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call57, i64 noundef %sub58)
  %call60 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call59, i8* noundef getelementptr inbounds ([2 x i8], [2 x i8]* @.str.1, i64 0, i64 0))
  ret i32 0
}

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_Z7readTSCv() #5 comdat {
entry:
  %0 = call i64 @llvm.x86.rdtsc()
  ret i64 %0
}

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #6

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #7

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), i8* noundef) #1

declare dso_local noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEy(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8), i64 noundef) #1

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main() #8 {
entry:
  %A = alloca [10 x [10 x [10 x i32]]], align 16
  %arraydecay = getelementptr inbounds [10 x [10 x [10 x i32]]], [10 x [10 x [10 x i32]]]* %A, i64 0, i64 0
  %call = call noundef i32 @_Z4loopPA10_A10_ii([10 x [10 x i32]]* noundef nonnull %arraydecay, i32 noundef 10)
  ret i32 0
}

; Function Attrs: nounwind
declare i64 @llvm.x86.rdtsc() #3

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_cilkfor_doublenest.cpp() #0 section ".text.startup" {
entry:
  call void @__cxx_global_var_init()
  ret void
}

attributes #0 = { noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }
attributes #4 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { argmemonly nounwind willreturn }
attributes #7 = { argmemonly willreturn }
attributes #8 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"}
!4 = distinct !{!4, !5}
!5 = !{!"tapir.loop.spawn.strategy", i32 1}
!6 = distinct !{!6, !5}
!7 = distinct !{!7, !5}
