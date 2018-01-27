// Tests for the cfi-vcall feature:
// RUN: %clang_cc1 -flto -triple x86_64-unknown-linux -fvisibility hidden -fsanitize=cfi-vcall -fsanitize-trap=cfi-vcall -emit-llvm -o - %s | FileCheck --check-prefix=CFI --check-prefix=CFI-NVT --check-prefix=ITANIUM --check-prefix=TT-ITANIUM --check-prefix=NDIAG %s
// RUN: %clang_cc1 -flto -triple x86_64-unknown-linux -fvisibility hidden -fsanitize=cfi-vcall -emit-llvm -o - %s | FileCheck --check-prefix=CFI --check-prefix=CFI-NVT --check-prefix=ITANIUM --check-prefix=TT-ITANIUM --check-prefix=ITANIUM-DIAG --check-prefix=DIAG --check-prefix=DIAG-ABORT %s
// RUN: %clang_cc1 -flto -triple x86_64-unknown-linux -fvisibility hidden -fsanitize=cfi-vcall -fsanitize-recover=cfi-vcall -emit-llvm -o - %s | FileCheck --check-prefix=CFI --check-prefix=CFI-NVT --check-prefix=ITANIUM --check-prefix=TT-ITANIUM --check-prefix=ITANIUM-DIAG --check-prefix=DIAG --check-prefix=DIAG-RECOVER %s
// RUN: %clang_cc1 -flto -triple x86_64-pc-windows-msvc -fsanitize=cfi-vcall -fsanitize-trap=cfi-vcall -emit-llvm -o - %s | FileCheck --check-prefix=CFI --check-prefix=CFI-NVT --check-prefix=MS --check-prefix=TT-MS --check-prefix=NDIAG %s

// Tests for the whole-program-vtables feature:
// RUN: %clang_cc1 -flto -triple x86_64-unknown-linux -fvisibility hidden -fwhole-program-vtables -emit-llvm -o - %s | FileCheck --check-prefix=VTABLE-OPT --check-prefix=ITANIUM --check-prefix=TT-ITANIUM %s
// RUN: %clang_cc1 -flto -triple x86_64-pc-windows-msvc -fwhole-program-vtables -emit-llvm -o - %s | FileCheck --check-prefix=VTABLE-OPT --check-prefix=MS --check-prefix=TT-MS %s

// Tests for cfi + whole-program-vtables:
// RUN: %clang_cc1 -flto -triple x86_64-unknown-linux -fvisibility hidden -fsanitize=cfi-vcall -fsanitize-trap=cfi-vcall -fwhole-program-vtables -emit-llvm -o - %s | FileCheck --check-prefix=CFI --check-prefix=CFI-VT --check-prefix=ITANIUM --check-prefix=TC-ITANIUM %s
// RUN: %clang_cc1 -flto -triple x86_64-pc-windows-msvc -fsanitize=cfi-vcall -fsanitize-trap=cfi-vcall -fwhole-program-vtables -emit-llvm -o - %s | FileCheck --check-prefix=CFI --check-prefix=CFI-VT --check-prefix=MS --check-prefix=TC-MS %s

// ITANIUM: @_ZTV1A = {{[^!]*}}, !type [[A16:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL16:![0-9]+]]

// ITANIUM: @_ZTV1B = {{[^!]*}}, !type [[A32:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL32:![0-9]+]]
// ITANIUM-SAME: !type [[B32:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]

// ITANIUM: @_ZTV1C = {{[^!]*}}, !type [[A32]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]
// ITANIUM-SAME: !type [[C32:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]

// DIAG: @[[SRC:.*]] = private unnamed_addr constant [{{.*}} x i8] c"{{.*}}type-metadata.cpp\00", align 1
// DIAG: @[[TYPE:.*]] = private unnamed_addr constant { i16, i16, [4 x i8] } { i16 -1, i16 0, [4 x i8] c"'A'\00" }
// DIAG: @[[BADTYPESTATIC:.*]] = private unnamed_addr global { i8, { [{{.*}} x i8]*, i32, i32 }, { i16, i16, [4 x i8] }* } { i8 0, { [{{.*}} x i8]*, i32, i32 } { [{{.*}} x i8]* @[[SRC]], i32 123, i32 3 }, { i16, i16, [4 x i8] }* @[[TYPE]] }

// ITANIUM: @_ZTVN12_GLOBAL__N_11DE = {{[^!]*}}, !type [[A32]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]
// ITANIUM-SAME: !type [[B32]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]
// ITANIUM-SAME: !type [[C88:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL88:![0-9]+]]
// ITANIUM-SAME: !type [[D32:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]

// ITANIUM: @_ZTCN12_GLOBAL__N_11DE0_1B = {{[^!]*}}, !type [[A32]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]
// ITANIUM-SAME: !type [[B32]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]

// ITANIUM: @_ZTCN12_GLOBAL__N_11DE8_1C = {{[^!]*}}, !type [[A64:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL64:![0-9]+]]
// ITANIUM-SAME: !type [[C32]]
// ITANIUM-DIAG-SAME: !type [[ALL32]]

// ITANIUM: @_ZTVZ3foovE2FA = {{[^!]*}}, !type [[A16]]
// ITANIUM-DIAG-SAME: !type [[ALL16]]
// ITANIUM-SAME: !type [[FA16:![0-9]+]]
// ITANIUM-DIAG-SAME: !type [[ALL16]]

// MS: comdat($"\01??_7A@@6B@"), !type [[A8:![0-9]+]]
// MS: comdat($"\01??_7B@@6B0@@"), !type [[B8:![0-9]+]]
// MS: comdat($"\01??_7B@@6BA@@@"), !type [[A8]]
// MS: comdat($"\01??_7C@@6B@"), !type [[A8]]
// MS: comdat($"\01??_7D@?A@@6BB@@@"), !type [[B8]], !type [[D8:![0-9]+]]
// MS: comdat($"\01??_7D@?A@@6BA@@@"), !type [[A8]]
// MS: comdat($"\01??_7FA@?1??foo@@YAXXZ@6B@"), !type [[A8]], !type [[FA8:![0-9]+]]

struct A {
  A();
  virtual void f();
};

struct B : virtual A {
  B();
  virtual void g();
  virtual void h();
};

struct C : virtual A {
  C();
};

namespace {

struct D : B, C {
  D();
  virtual void f();
  virtual void h();
};

}

A::A() {}
B::B() {}
C::C() {}
D::D() {}

void A::f() {
}

void B::g() {
}

void D::f() {
}

void D::h() {
}

// ITANIUM: define hidden void @_Z2afP1A
// MS: define void @"\01?af@@YAXPEAUA@@@Z"
void af(A *a) {
  // TT-ITANIUM: [[P:%[^ ]*]] = call i1 @llvm.type.test(i8* [[VT:%[^ ]*]], metadata !"_ZTS1A")
  // TT-MS: [[P:%[^ ]*]] = call i1 @llvm.type.test(i8* [[VT:%[^ ]*]], metadata !"?AUA@@")
  // TC-ITANIUM: [[PAIR:%[^ ]*]] = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 0, metadata !"_ZTS1A")
  // TC-MS: [[PAIR:%[^ ]*]] = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 0, metadata !"?AUA@@")
  // CFI-VT: [[P:%[^ ]*]] = extractvalue { i8*, i1 } [[PAIR]], 1
  // DIAG-NEXT: [[VTVALID0:%[^ ]*]] = call i1 @llvm.type.test(i8* [[VT]], metadata !"all-vtables")
  // VTABLE-OPT: call void @llvm.assume(i1 [[P]])
  // CFI-NEXT: br i1 [[P]], label %[[CONTBB:[^ ,]*]], label %[[TRAPBB:[^ ,]*]]
  // CFI-NEXT: {{^$}}

  // CFI: [[TRAPBB]]
  // NDIAG-NEXT: call void @llvm.trap()
  // NDIAG-NEXT: unreachable
  // DIAG-NEXT: [[VTINT:%[^ ]*]] = ptrtoint i8* [[VT]] to i64
  // DIAG-NEXT: [[VTVALID:%[^ ]*]] = zext i1 [[VTVALID0]] to i64
  // DIAG-ABORT-NEXT: call void @__ubsan_handle_cfi_check_fail_abort(i8* getelementptr inbounds ({{.*}} @[[BADTYPESTATIC]], i32 0, i32 0), i64 [[VTINT]], i64 [[VTVALID]])
  // DIAG-ABORT-NEXT: unreachable
  // DIAG-RECOVER-NEXT: call void @__ubsan_handle_cfi_check_fail(i8* getelementptr inbounds ({{.*}} @[[BADTYPESTATIC]], i32 0, i32 0), i64 [[VTINT]], i64 [[VTVALID]])
  // DIAG-RECOVER-NEXT: br label %[[CONTBB]]

  // CFI: [[CONTBB]]
  // CFI-NVT: [[PTR:%[^ ]*]] = load
  // CFI-VT: [[PTRI8:%[^ ]*]] = extractvalue { i8*, i1 } [[PAIR]], 0
  // CFI-VT: [[PTR:%[^ ]*]] = bitcast i8* [[PTRI8]] to
  // CFI: call void [[PTR]]
#line 123
  a->f();
}

// ITANIUM: define internal void @_Z3df1PN12_GLOBAL__N_11DE
// MS: define internal void @"\01?df1@@YAXPEAUD@?A@@@Z"
void df1(D *d) {
  // TT-ITANIUM: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata ![[DTYPE:[0-9]+]])
  // TT-MS: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata !"?AUA@@")
  // TC-ITANIUM: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 0, metadata ![[DTYPE:[0-9]+]])
  // TC-MS: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 0, metadata !"?AUA@@")
  d->f();
}

// ITANIUM: define internal void @_Z3dg1PN12_GLOBAL__N_11DE
// MS: define internal void @"\01?dg1@@YAXPEAUD@?A@@@Z"
void dg1(D *d) {
  // TT-ITANIUM: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata !"_ZTS1B")
  // TT-MS: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata !"?AUB@@")
  // TC-ITANIUM: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 8, metadata !"_ZTS1B")
  // TC-MS: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 0, metadata !"?AUB@@")
  d->g();
}

// ITANIUM: define internal void @_Z3dh1PN12_GLOBAL__N_11DE
// MS: define internal void @"\01?dh1@@YAXPEAUD@?A@@@Z"
void dh1(D *d) {
  // TT-ITANIUM: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata ![[DTYPE]])
  // TT-MS: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata ![[DTYPE:[0-9]+]])
  // TC-ITANIUM: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 16, metadata ![[DTYPE]])
  // TC-MS: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 8, metadata ![[DTYPE:[0-9]+]])
  d->h();
}

// ITANIUM: define internal void @_Z3df2PN12_GLOBAL__N_11DE
// MS: define internal void @"\01?df2@@YAXPEAUD@?A@@@Z"
__attribute__((no_sanitize("cfi")))
void df2(D *d) {
  // CFI-NVT-NOT: call i1 @llvm.type.test
  // CFI-VT: [[P:%[^ ]*]] = call i1 @llvm.type.test
  // CFI-VT: call void @llvm.assume(i1 [[P]])
  d->f();
}

// ITANIUM: define internal void @_Z3df3PN12_GLOBAL__N_11DE
// MS: define internal void @"\01?df3@@YAXPEAUD@?A@@@Z"
__attribute__((no_sanitize("address"))) __attribute__((no_sanitize("cfi-vcall")))
void df3(D *d) {
  // CFI-NVT-NOT: call i1 @llvm.type.test
  // CFI-VT: [[P:%[^ ]*]] = call i1 @llvm.type.test
  // CFI-VT: call void @llvm.assume(i1 [[P]])
  d->f();
}

D d;

void foo() {
  df1(&d);
  dg1(&d);
  dh1(&d);
  df2(&d);
  df3(&d);

  struct FA : A {
    void f() {}
  } fa;
  af(&fa);
}

namespace test2 {

struct A {
  virtual void m_fn1();
};
struct B {
  virtual void m_fn2();
};
struct C : B, A {};
struct D : C {
  void m_fn1();
};

// ITANIUM: define hidden void @_ZN5test21fEPNS_1DE
// MS: define void @"\01?f@test2@@YAXPEAUD@1@@Z"
void f(D *d) {
  // TT-ITANIUM: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata !"_ZTSN5test21DE")
  // TT-MS: {{%[^ ]*}} = call i1 @llvm.type.test(i8* {{%[^ ]*}}, metadata !"?AUA@test2@@")
  // TC-ITANIUM: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 8, metadata !"_ZTSN5test21DE")
  // TC-MS: {{%[^ ]*}} = call { i8*, i1 } @llvm.type.checked.load(i8* {{%[^ ]*}}, i32 0, metadata !"?AUA@test2@@")
  d->m_fn1();
}

}

// ITANIUM: [[A16]] = !{i64 16, !"_ZTS1A"}
// ITANIUM-DIAG: [[ALL16]] = !{i64 16, !"all-vtables"}
// ITANIUM: [[A32]] = !{i64 32, !"_ZTS1A"}
// ITANIUM-DIAG: [[ALL32]] = !{i64 32, !"all-vtables"}
// ITANIUM: [[B32]] = !{i64 32, !"_ZTS1B"}
// ITANIUM: [[C32]] = !{i64 32, !"_ZTS1C"}
// ITANIUM: [[C88]] = !{i64 88, !"_ZTS1C"}
// ITANIUM-DIAG: [[ALL88]] = !{i64 88, !"all-vtables"}
// ITANIUM: [[D32]] = !{i64 32, [[D_ID:![0-9]+]]}
// ITANIUM: [[D_ID]] = distinct !{}
// ITANIUM: [[A64]] = !{i64 64, !"_ZTS1A"}
// ITANIUM-DIAG: [[ALL64]] = !{i64 64, !"all-vtables"}
// ITANIUM: [[FA16]] = !{i64 16, [[FA_ID:![0-9]+]]}
// ITANIUM: [[FA_ID]] = distinct !{}

// MS: [[A8]] = !{i64 8, !"?AUA@@"}
// MS: [[B8]] = !{i64 8, !"?AUB@@"}
// MS: [[D8]] = !{i64 8, [[D_ID:![0-9]+]]}
// MS: [[D_ID]] = distinct !{}
// MS: [[FA8]] = !{i64 8, [[FA_ID:![0-9]+]]}
// MS: [[FA_ID]] = distinct !{}
