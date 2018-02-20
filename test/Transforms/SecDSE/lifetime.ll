; RUN: opt -S -basicaa -dse < %s | FileCheck %s

target datalayout = "E-p:64:64:64-a0:0:8-f32:32:32-f64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-v64:64:64-v128:128:128"

declare void @llvm.lifetime.start(i64, i8* nocapture) nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) nounwind
declare void @llvm.memset.p0i8.i8(i8* nocapture, i8, i8, i32, i1) nounwind

define void @test1() #1 {
; CHECK-LABEL: @test1(
  %A = alloca i8

  store i8 0, i8* %A  ;; Written to by memset
  call void @llvm.lifetime.end(i64 1, i8* %A)
; CHECK: lifetime.end

  call void @llvm.memset.p0i8.i8(i8* %A, i8 0, i8 -1, i32 0, i1 false)
; CHECK: memset

  ret void
; CHECK: ret void
}

define void @test2(i32* %P) #1 {
; CHECK-LABEL: @test2(
  %Q = getelementptr i32, i32* %P, i32 1
  %R = bitcast i32* %Q to i8*
  call void @llvm.lifetime.start(i64 4, i8* %R)
; CHECK: lifetime.start
  store i32 0, i32* %Q  ;; This store is NOT dead.
; CHECK: store
  call void @llvm.lifetime.end(i64 4, i8* %R)
; CHECK: lifetime.end
  ret void
}

define void @test3() #1 {
; CHECK-LABEL: @test3(
  %A = alloca i8

  store i8 0, i8* %A  ;; Written to by memset
  call void @llvm.lifetime.end(i64 1, i8* %A)
; CHECK: lifetime.end

  store i8 0, i8* %A ;; This store is dead.
; CHECK-NOT: store

  call void @llvm.memset.p0i8.i8(i8* %A, i8 0, i8 -1, i32 0, i1 false) ;; This memset is NOT dead because it may be a scrubbing operation.
; CHECK: memset

  ret void
; CHECK: ret void
}

define void @test4(i32* %P) #1 {
; CHECK-LABEL: @test4(
  %Q = getelementptr i32, i32* %P, i32 1
  %R = bitcast i32* %Q to i8*
  call void @llvm.lifetime.start(i64 4, i8* %R)
; CHECK: lifetime.start
  store i32 0, i32* %Q  ;; This store is dead.
  store i32 0, i32* %Q  ;; This store is NOT dead.
; CHECK-NEXT: store
  call void @llvm.lifetime.end(i64 4, i8* %R)
; CHECK-NEXT: lifetime.end
  ret void
}

attributes #1 = { secdse }
