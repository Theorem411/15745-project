; ModuleID = 'verlet.bc'
source_filename = "verlet.cpp"
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
%class.VerletSimulation = type <{ %struct.Particle*, i32, [4 x i8] }>
%struct.Particle = type { double, double, double, double, double, double, double, double, double }
%"struct.std::chrono::time_point" = type { %"struct.std::chrono::duration" }
%"struct.std::chrono::duration" = type { i64 }
%"struct.std::chrono::duration.0" = type { double }

$_ZN16VerletSimulationC2Ei = comdat any

$_ZN16VerletSimulation8simulateEi = comdat any

$_ZN16VerletSimulationD2Ev = comdat any

$_ZN16VerletSimulation19initializeParticlesEv = comdat any

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
@.str = private unnamed_addr constant [37 x i8] c"Verlet Integration Simulation Time: \00", align 1
@.str.1 = private unnamed_addr constant [10 x i8] c" seconds\0A\00", align 1
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_verlet.cpp, i8* null }]

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

; Function Attrs: mustprogress noinline norecurse uwtable
define dso_local noundef i32 @main() #4 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %verletSimulation = alloca %class.VerletSimulation, align 8
  call void @_ZN16VerletSimulationC2Ei(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %verletSimulation, i32 noundef 500)
  invoke void @_ZN16VerletSimulation8simulateEi(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %verletSimulation, i32 noundef 30)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  call void @_ZN16VerletSimulationD2Ev(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %verletSimulation) #3
  ret i32 0

lpad:                                             ; preds = %entry
  %0 = landingpad { i8*, i32 }
          cleanup
  call void @_ZN16VerletSimulationD2Ev(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %verletSimulation) #3
  br label %eh.resume

eh.resume:                                        ; preds = %lpad
  resume { i8*, i32 } %0
}

; Function Attrs: noinline uwtable
define linkonce_odr dso_local void @_ZN16VerletSimulationC2Ei(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %this, i32 noundef %numParticles) unnamed_addr #0 comdat align 2 {
entry:
  %conv = sext i32 %numParticles to i64
  %0 = call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %conv, i64 72)
  %1 = extractvalue { i64, i1 } %0, 1
  %2 = extractvalue { i64, i1 } %0, 0
  %3 = select i1 %1, i64 -1, i64 %2
  %call = call noalias noundef nonnull i8* @_Znam(i64 noundef %3) #14
  %4 = bitcast %class.VerletSimulation* %this to i8**
  store i8* %call, i8** %4, align 8
  %numParticles_ = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 1
  store i32 %numParticles, i32* %numParticles_, align 8
  call void @_ZN16VerletSimulation19initializeParticlesEv(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %this)
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local void @_ZN16VerletSimulation8simulateEi(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %this, i32 noundef %numIterations) #5 comdat align 2 {
entry:
  %start = alloca %"struct.std::chrono::time_point", align 8
  %syncreg = call token @llvm.syncregion.start()
  %end = alloca %"struct.std::chrono::time_point", align 8
  %duration = alloca %"struct.std::chrono::duration.0", align 8
  %ref.tmp = alloca %"struct.std::chrono::duration", align 8
  %call = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive2 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %start, i64 0, i32 0, i32 0
  store i64 %call, i64* %coerce.dive2, align 8
  %cmp4 = icmp sgt i32 %numIterations, 0
  br i1 %cmp4, label %for.body.lr.ph, label %for.end123

for.body.lr.ph:                                   ; preds = %entry
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc121
  %iter.05 = phi i32 [ 0, %for.body.lr.ph ], [ %inc122, %for.inc121 ]
  %numParticles_ = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 1
  %0 = load i32, i32* %numParticles_, align 8
  %cmp3 = icmp sgt i32 %0, 0
  br i1 %cmp3, label %pfor.ph, label %pfor.end

pfor.ph:                                          ; preds = %for.body
  br label %pfor.cond

pfor.cond:                                        ; preds = %pfor.inc, %pfor.ph
  %__begin.0 = phi i32 [ 0, %pfor.ph ], [ %inc119, %pfor.inc ]
  br label %pfor.detach

pfor.detach:                                      ; preds = %pfor.cond
  detach within %syncreg, label %pfor.body.entry, label %pfor.inc

pfor.body.entry:                                  ; preds = %pfor.detach
  br label %pfor.body

pfor.body:                                        ; preds = %pfor.body.entry
  %particles = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %1 = load %struct.Particle*, %struct.Particle** %particles, align 8
  %idxprom = zext i32 %__begin.0 to i64
  %ax = getelementptr inbounds %struct.Particle, %struct.Particle* %1, i64 %idxprom, i32 6
  store double 0.000000e+00, double* %ax, align 8
  %particles6 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %2 = load %struct.Particle*, %struct.Particle** %particles6, align 8
  %idxprom7 = zext i32 %__begin.0 to i64
  %ay = getelementptr inbounds %struct.Particle, %struct.Particle* %2, i64 %idxprom7, i32 7
  store double 0.000000e+00, double* %ay, align 8
  %particles9 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %3 = load %struct.Particle*, %struct.Particle** %particles9, align 8
  %idxprom10 = zext i32 %__begin.0 to i64
  %az = getelementptr inbounds %struct.Particle, %struct.Particle* %3, i64 %idxprom10, i32 8
  store double 0.000000e+00, double* %az, align 8
  %numParticles_13 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 1
  %4 = load i32, i32* %numParticles_13, align 8
  %cmp141 = icmp sgt i32 %4, 0
  br i1 %cmp141, label %for.body15.lr.ph, label %for.end

for.body15.lr.ph:                                 ; preds = %pfor.body
  br label %for.body15

for.body15:                                       ; preds = %for.body15.lr.ph, %for.inc
  %j.02 = phi i32 [ 0, %for.body15.lr.ph ], [ %inc, %for.inc ]
  %cmp16.not = icmp eq i32 %__begin.0, %j.02
  br i1 %cmp16.not, label %if.end, label %if.then

if.then:                                          ; preds = %for.body15
  %particles17 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %5 = load %struct.Particle*, %struct.Particle** %particles17, align 8
  %idxprom18 = zext i32 %j.02 to i64
  %x = getelementptr inbounds %struct.Particle, %struct.Particle* %5, i64 %idxprom18, i32 0
  %6 = load double, double* %x, align 8
  %idxprom21 = zext i32 %__begin.0 to i64
  %x23 = getelementptr inbounds %struct.Particle, %struct.Particle* %5, i64 %idxprom21, i32 0
  %7 = load double, double* %x23, align 8
  %sub24 = fsub double %6, %7
  %particles25 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %8 = load %struct.Particle*, %struct.Particle** %particles25, align 8
  %idxprom26 = zext i32 %j.02 to i64
  %y = getelementptr inbounds %struct.Particle, %struct.Particle* %8, i64 %idxprom26, i32 1
  %9 = load double, double* %y, align 8
  %idxprom29 = zext i32 %__begin.0 to i64
  %y31 = getelementptr inbounds %struct.Particle, %struct.Particle* %8, i64 %idxprom29, i32 1
  %10 = load double, double* %y31, align 8
  %sub32 = fsub double %9, %10
  %particles33 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %11 = load %struct.Particle*, %struct.Particle** %particles33, align 8
  %idxprom34 = zext i32 %j.02 to i64
  %z = getelementptr inbounds %struct.Particle, %struct.Particle* %11, i64 %idxprom34, i32 2
  %12 = load double, double* %z, align 8
  %idxprom37 = zext i32 %__begin.0 to i64
  %z39 = getelementptr inbounds %struct.Particle, %struct.Particle* %11, i64 %idxprom37, i32 2
  %13 = load double, double* %z39, align 8
  %sub40 = fsub double %12, %13
  %mul42 = fmul double %sub32, %sub32
  %14 = call double @llvm.fmuladd.f64(double %sub24, double %sub24, double %mul42)
  %15 = call double @llvm.fmuladd.f64(double %sub40, double %sub40, double %14)
  %add44 = fadd double %15, 1.000000e+00
  %call45 = call double @sqrt(double noundef %add44) #3
  %div46 = fdiv double 1.000000e+00, %add44
  %mul47 = fmul double %div46, %sub24
  %div48 = fdiv double %mul47, %call45
  %mul49 = fmul double %div46, %sub32
  %div50 = fdiv double %mul49, %call45
  %mul51 = fmul double %div46, %sub40
  %div52 = fdiv double %mul51, %call45
  %particles53 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %16 = load %struct.Particle*, %struct.Particle** %particles53, align 8
  %idxprom54 = zext i32 %__begin.0 to i64
  %ax56 = getelementptr inbounds %struct.Particle, %struct.Particle* %16, i64 %idxprom54, i32 6
  %17 = load double, double* %ax56, align 8
  %add57 = fadd double %17, %div48
  store double %add57, double* %ax56, align 8
  %particles58 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %18 = load %struct.Particle*, %struct.Particle** %particles58, align 8
  %idxprom59 = zext i32 %__begin.0 to i64
  %ay61 = getelementptr inbounds %struct.Particle, %struct.Particle* %18, i64 %idxprom59, i32 7
  %19 = load double, double* %ay61, align 8
  %add62 = fadd double %19, %div50
  store double %add62, double* %ay61, align 8
  %particles63 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %20 = load %struct.Particle*, %struct.Particle** %particles63, align 8
  %idxprom64 = zext i32 %__begin.0 to i64
  %az66 = getelementptr inbounds %struct.Particle, %struct.Particle* %20, i64 %idxprom64, i32 8
  %21 = load double, double* %az66, align 8
  %add67 = fadd double %21, %div52
  store double %add67, double* %az66, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %for.body15
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %inc = add nuw nsw i32 %j.02, 1
  %22 = load i32, i32* %numParticles_13, align 8
  %cmp14 = icmp slt i32 %inc, %22
  br i1 %cmp14, label %for.body15, label %for.cond12.for.end_crit_edge, !llvm.loop !4

for.cond12.for.end_crit_edge:                     ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond12.for.end_crit_edge, %pfor.body
  %particles68 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %23 = load %struct.Particle*, %struct.Particle** %particles68, align 8
  %idxprom69 = zext i32 %__begin.0 to i64
  %ax71 = getelementptr inbounds %struct.Particle, %struct.Particle* %23, i64 %idxprom69, i32 6
  %24 = load double, double* %ax71, align 8
  %idxprom74 = zext i32 %__begin.0 to i64
  %vx = getelementptr inbounds %struct.Particle, %struct.Particle* %23, i64 %idxprom74, i32 3
  %25 = load double, double* %vx, align 8
  %26 = call double @llvm.fmuladd.f64(double %24, double 1.000000e-03, double %25)
  store double %26, double* %vx, align 8
  %particles76 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %27 = load %struct.Particle*, %struct.Particle** %particles76, align 8
  %idxprom77 = zext i32 %__begin.0 to i64
  %ay79 = getelementptr inbounds %struct.Particle, %struct.Particle* %27, i64 %idxprom77, i32 7
  %28 = load double, double* %ay79, align 8
  %idxprom82 = zext i32 %__begin.0 to i64
  %vy = getelementptr inbounds %struct.Particle, %struct.Particle* %27, i64 %idxprom82, i32 4
  %29 = load double, double* %vy, align 8
  %30 = call double @llvm.fmuladd.f64(double %28, double 1.000000e-03, double %29)
  store double %30, double* %vy, align 8
  %particles84 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %31 = load %struct.Particle*, %struct.Particle** %particles84, align 8
  %idxprom85 = zext i32 %__begin.0 to i64
  %az87 = getelementptr inbounds %struct.Particle, %struct.Particle* %31, i64 %idxprom85, i32 8
  %32 = load double, double* %az87, align 8
  %idxprom90 = zext i32 %__begin.0 to i64
  %vz = getelementptr inbounds %struct.Particle, %struct.Particle* %31, i64 %idxprom90, i32 5
  %33 = load double, double* %vz, align 8
  %34 = call double @llvm.fmuladd.f64(double %32, double 1.000000e-03, double %33)
  store double %34, double* %vz, align 8
  %particles92 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %35 = load %struct.Particle*, %struct.Particle** %particles92, align 8
  %idxprom93 = zext i32 %__begin.0 to i64
  %vx95 = getelementptr inbounds %struct.Particle, %struct.Particle* %35, i64 %idxprom93, i32 3
  %36 = load double, double* %vx95, align 8
  %idxprom98 = zext i32 %__begin.0 to i64
  %x100 = getelementptr inbounds %struct.Particle, %struct.Particle* %35, i64 %idxprom98, i32 0
  %37 = load double, double* %x100, align 8
  %38 = call double @llvm.fmuladd.f64(double %36, double 1.000000e-03, double %37)
  store double %38, double* %x100, align 8
  %particles101 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %39 = load %struct.Particle*, %struct.Particle** %particles101, align 8
  %idxprom102 = zext i32 %__begin.0 to i64
  %vy104 = getelementptr inbounds %struct.Particle, %struct.Particle* %39, i64 %idxprom102, i32 4
  %40 = load double, double* %vy104, align 8
  %idxprom107 = zext i32 %__begin.0 to i64
  %y109 = getelementptr inbounds %struct.Particle, %struct.Particle* %39, i64 %idxprom107, i32 1
  %41 = load double, double* %y109, align 8
  %42 = call double @llvm.fmuladd.f64(double %40, double 1.000000e-03, double %41)
  store double %42, double* %y109, align 8
  %particles110 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %43 = load %struct.Particle*, %struct.Particle** %particles110, align 8
  %idxprom111 = zext i32 %__begin.0 to i64
  %vz113 = getelementptr inbounds %struct.Particle, %struct.Particle* %43, i64 %idxprom111, i32 5
  %44 = load double, double* %vz113, align 8
  %idxprom116 = zext i32 %__begin.0 to i64
  %z118 = getelementptr inbounds %struct.Particle, %struct.Particle* %43, i64 %idxprom116, i32 2
  %45 = load double, double* %z118, align 8
  %46 = call double @llvm.fmuladd.f64(double %44, double 1.000000e-03, double %45)
  store double %46, double* %z118, align 8
  br label %pfor.preattach

pfor.preattach:                                   ; preds = %for.end
  reattach within %syncreg, label %pfor.inc

pfor.inc:                                         ; preds = %pfor.preattach, %pfor.detach
  %inc119 = add nuw nsw i32 %__begin.0, 1
  %cmp120 = icmp slt i32 %inc119, %0
  br i1 %cmp120, label %pfor.cond, label %pfor.cond.cleanup, !llvm.loop !6

pfor.cond.cleanup:                                ; preds = %pfor.inc
  sync within %syncreg, label %sync.continue

sync.continue:                                    ; preds = %pfor.cond.cleanup
  call void @llvm.sync.unwind(token %syncreg)
  br label %pfor.end

pfor.end:                                         ; preds = %sync.continue, %for.body
  br label %for.inc121

for.inc121:                                       ; preds = %pfor.end
  %inc122 = add nuw nsw i32 %iter.05, 1
  %cmp = icmp slt i32 %inc122, %numIterations
  br i1 %cmp, label %for.body, label %for.cond.for.end123_crit_edge, !llvm.loop !8

for.cond.for.end123_crit_edge:                    ; preds = %for.inc121
  br label %for.end123

for.end123:                                       ; preds = %for.cond.for.end123_crit_edge, %entry
  %call124 = call i64 @_ZNSt6chrono3_V212system_clock3nowEv() #3
  %coerce.dive126 = getelementptr inbounds %"struct.std::chrono::time_point", %"struct.std::chrono::time_point"* %end, i64 0, i32 0, i32 0
  store i64 %call124, i64* %coerce.dive126, align 8
  %call127 = call i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %end, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %start)
  %coerce.dive128 = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %ref.tmp, i64 0, i32 0
  store i64 %call127, i64* %coerce.dive128, align 8
  call void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IlS1_ILl1ELl1000000000EEvEERKNS0_IT_T0_EE(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %ref.tmp)
  %call129 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i8* noundef getelementptr inbounds ([37 x i8], [37 x i8]* @.str, i64 0, i64 0))
  %call130 = call noundef double @_ZNKSt6chrono8durationIdSt5ratioILl1ELl1EEE5countEv(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %duration)
  %call131 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call129, double noundef %call130)
  %call132 = call noundef nonnull align 8 dereferenceable(8) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* noundef nonnull align 8 dereferenceable(8) %call131, i8* noundef getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i64 0, i64 0))
  ret void
}

declare dso_local i32 @__gxx_personality_v0(...)

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZN16VerletSimulationD2Ev(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %this) unnamed_addr #6 comdat align 2 {
entry:
  %particles = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %0 = load %struct.Particle*, %struct.Particle** %particles, align 8
  %isnull = icmp eq %struct.Particle* %0, null
  br i1 %isnull, label %delete.end, label %delete.notnull

delete.notnull:                                   ; preds = %entry
  %1 = bitcast %struct.Particle* %0 to i8*
  call void @_ZdaPv(i8* noundef %1) #15
  br label %delete.end

delete.end:                                       ; preds = %delete.notnull, %entry
  ret void
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare { i64, i1 } @llvm.umul.with.overflow.i64(i64, i64) #7

; Function Attrs: nobuiltin allocsize(0)
declare dso_local noundef nonnull i8* @_Znam(i64 noundef) #8

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local void @_ZN16VerletSimulation19initializeParticlesEv(%class.VerletSimulation* noundef nonnull align 8 dereferenceable(12) %this) #9 comdat align 2 {
entry:
  %call = call i64 @time(i64* noundef null) #3
  %conv = trunc i64 %call to i32
  call void @srand(i32 noundef %conv) #3
  %numParticles_ = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 1
  %0 = load i32, i32* %numParticles_, align 8
  %cmp1 = icmp sgt i32 %0, 0
  br i1 %cmp1, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %entry
  br label %for.body

for.body:                                         ; preds = %for.body.lr.ph, %for.inc
  %i.02 = phi i32 [ 0, %for.body.lr.ph ], [ %inc, %for.inc ]
  %call2 = call i32 @rand() #3
  %conv3 = sitofp i32 %call2 to double
  %div = fdiv double %conv3, 0x41DFFFFFFFC00000
  %particles = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %1 = load %struct.Particle*, %struct.Particle** %particles, align 8
  %idxprom = zext i32 %i.02 to i64
  %x = getelementptr inbounds %struct.Particle, %struct.Particle* %1, i64 %idxprom, i32 0
  store double %div, double* %x, align 8
  %call4 = call i32 @rand() #3
  %conv5 = sitofp i32 %call4 to double
  %div6 = fdiv double %conv5, 0x41DFFFFFFFC00000
  %particles7 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %2 = load %struct.Particle*, %struct.Particle** %particles7, align 8
  %idxprom8 = zext i32 %i.02 to i64
  %y = getelementptr inbounds %struct.Particle, %struct.Particle* %2, i64 %idxprom8, i32 1
  store double %div6, double* %y, align 8
  %call10 = call i32 @rand() #3
  %conv11 = sitofp i32 %call10 to double
  %div12 = fdiv double %conv11, 0x41DFFFFFFFC00000
  %particles13 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %3 = load %struct.Particle*, %struct.Particle** %particles13, align 8
  %idxprom14 = zext i32 %i.02 to i64
  %z = getelementptr inbounds %struct.Particle, %struct.Particle* %3, i64 %idxprom14, i32 2
  store double %div12, double* %z, align 8
  %call16 = call i32 @rand() #3
  %conv17 = sitofp i32 %call16 to double
  %div18 = fdiv double %conv17, 0x41DFFFFFFFC00000
  %particles19 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %4 = load %struct.Particle*, %struct.Particle** %particles19, align 8
  %idxprom20 = zext i32 %i.02 to i64
  %vx = getelementptr inbounds %struct.Particle, %struct.Particle* %4, i64 %idxprom20, i32 3
  store double %div18, double* %vx, align 8
  %call22 = call i32 @rand() #3
  %conv23 = sitofp i32 %call22 to double
  %div24 = fdiv double %conv23, 0x41DFFFFFFFC00000
  %particles25 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %5 = load %struct.Particle*, %struct.Particle** %particles25, align 8
  %idxprom26 = zext i32 %i.02 to i64
  %vy = getelementptr inbounds %struct.Particle, %struct.Particle* %5, i64 %idxprom26, i32 4
  store double %div24, double* %vy, align 8
  %call28 = call i32 @rand() #3
  %conv29 = sitofp i32 %call28 to double
  %div30 = fdiv double %conv29, 0x41DFFFFFFFC00000
  %particles31 = getelementptr inbounds %class.VerletSimulation, %class.VerletSimulation* %this, i64 0, i32 0
  %6 = load %struct.Particle*, %struct.Particle** %particles31, align 8
  %idxprom32 = zext i32 %i.02 to i64
  %vz = getelementptr inbounds %struct.Particle, %struct.Particle* %6, i64 %idxprom32, i32 5
  store double %div30, double* %vz, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nuw nsw i32 %i.02, 1
  %7 = load i32, i32* %numParticles_, align 8
  %cmp = icmp slt i32 %inc, %7
  br i1 %cmp, label %for.body, label %for.cond.for.end_crit_edge, !llvm.loop !9

for.cond.for.end_crit_edge:                       ; preds = %for.inc
  br label %for.end

for.end:                                          ; preds = %for.cond.for.end_crit_edge, %entry
  ret void
}

; Function Attrs: nounwind
declare dso_local void @srand(i32 noundef) #2

; Function Attrs: nounwind
declare dso_local i64 @time(i64* noundef) #2

; Function Attrs: nounwind
declare dso_local i32 @rand() #2

; Function Attrs: nounwind
declare dso_local i64 @_ZNSt6chrono3_V212system_clock3nowEv() #2

; Function Attrs: argmemonly nounwind willreturn
declare token @llvm.syncregion.start() #10

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare double @llvm.fmuladd.f64(double, double, double) #7

; Function Attrs: nounwind
declare dso_local double @sqrt(double noundef) #2

; Function Attrs: argmemonly willreturn
declare void @llvm.sync.unwind(token) #11

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local i64 @_ZNSt6chronomiINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEES6_EENSt11common_typeIJT0_T1_EE4typeERKNS_10time_pointIT_S8_EERKNSC_ISD_S9_EE(%"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__lhs, %"struct.std::chrono::time_point"* noundef nonnull align 8 dereferenceable(8) %__rhs) #5 comdat {
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
define linkonce_odr dso_local i64 @_ZNSt6chronomiIlSt5ratioILl1ELl1000000000EElS2_EENSt11common_typeIJNS_8durationIT_T0_EENS4_IT1_T2_EEEE4typeERKS7_RKSA_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__lhs, %"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__rhs) #5 comdat {
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
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #12

; Function Attrs: mustprogress noinline nounwind uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt6chrono8durationIlSt5ratioILl1ELl1000000000EEE5countEv(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this) #9 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__r, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind uwtable
define linkonce_odr dso_local void @_ZNSt6chrono8durationIlSt5ratioILl1ELl1000000000EEEC2IlvEERKT_(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %this, i64* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #6 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration", %"struct.std::chrono::duration"* %this, i64 0, i32 0
  %0 = load i64, i64* %__rep, align 8
  store i64 %0, i64* %__r, align 8
  ret void
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local double @_ZNSt6chrono13duration_castINS_8durationIdSt5ratioILl1ELl1EEEElS2_ILl1ELl1000000000EEEENSt9enable_ifIXsr13__is_durationIT_EE5valueES7_E4typeERKNS1_IT0_T1_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) #5 comdat {
entry:
  %call = call double @_ZNSt6chrono20__duration_cast_implINS_8durationIdSt5ratioILl1ELl1EEEES2_ILl1ELl1000000000EEdLb1ELb0EE6__castIlS5_EES4_RKNS1_IT_T0_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d)
  ret double %call
}

; Function Attrs: mustprogress noinline uwtable
define linkonce_odr dso_local double @_ZNSt6chrono20__duration_cast_implINS_8durationIdSt5ratioILl1ELl1EEEES2_ILl1ELl1000000000EEdLb1ELb0EE6__castIlS5_EES4_RKNS1_IT_T0_EE(%"struct.std::chrono::duration"* noundef nonnull align 8 dereferenceable(8) %__d) #5 comdat align 2 {
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
define linkonce_odr dso_local void @_ZNSt6chrono8durationIdSt5ratioILl1ELl1EEEC2IdvEERKT_(%"struct.std::chrono::duration.0"* noundef nonnull align 8 dereferenceable(8) %this, double* noundef nonnull align 8 dereferenceable(8) %__rep) unnamed_addr #6 comdat align 2 {
entry:
  %__r = getelementptr inbounds %"struct.std::chrono::duration.0", %"struct.std::chrono::duration.0"* %this, i64 0, i32 0
  %0 = load double, double* %__rep, align 8
  store double %0, double* %__r, align 8
  ret void
}

; Function Attrs: nobuiltin nounwind
declare dso_local void @_ZdaPv(i8* noundef) #13

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_verlet.cpp() #0 section ".text.startup" {
entry:
  call void @__cxx_global_var_init()
  ret void
}

attributes #0 = { noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }
attributes #4 = { mustprogress noinline norecurse uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #8 = { nobuiltin allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { argmemonly nounwind willreturn }
attributes #11 = { argmemonly willreturn }
attributes #12 = { argmemonly nofree nounwind willreturn }
attributes #13 = { nobuiltin nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #14 = { builtin allocsize(0) }
attributes #15 = { builtin nounwind }

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
!9 = distinct !{!9, !5}
