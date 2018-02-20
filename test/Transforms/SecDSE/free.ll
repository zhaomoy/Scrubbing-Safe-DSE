; RUN: opt < %s -basicaa -dse -S | FileCheck %s

target datalayout = "e-p:64:64:64"

declare void @free(i8* nocapture)
declare noalias i8* @malloc(i64)

; CHECK-LABEL: @test(
; CHECK-NOT: store
; CHECK: @free
; CHECK-NEXT: ret void
define void @test(i32* %Q, i32* %P) #1 {
        %DEAD = load i32, i32* %Q            ;; <i32> [#uses=1]
        store i32 %DEAD, i32* %P             ;; This is dead because the value is not a constant so secdse doesn't keep it.
        %1 = bitcast i32* %P to i8*
        tail call void @free(i8* %1)
        ret void
}

; CHECK-LABEL: @test2(
; CHECK: store
; CHECK: @free
; CHECK-NEXT: ret void
define void @test2({i32, i32}* %P) #1 {
	%Q = getelementptr {i32, i32}, {i32, i32} *%P, i32 0, i32 1
	store i32 4, i32* %Q        ;; This is not dead
        %1 = bitcast {i32, i32}* %P to i8*
        tail call void @free(i8* %1)
	ret void
}

; CHECK-LABEL: @test3(
; CHECK: store
; CHECK-NEXT: getelementptr
; CHECK-NEXT: store
; CHECK: ret void
define void @test3() #1 {
  %m = call i8* @malloc(i64 24)
  store i8 0, i8* %m
  %m1 = getelementptr i8, i8* %m, i64 1
  store i8 1, i8* %m1
  call void @free(i8* %m)
  ret void
}

; PR11240
; CHECK-LABEL: @test4(
; CHECK: store
; CHECK: @free
define void @test4(i1 %x) #2 {
entry:
  %alloc1 = tail call noalias i8* @malloc(i64 4) nounwind
  br i1 %x, label %skipinit1, label %init1

init1:
  store i8 1, i8* %alloc1
  br label %skipinit1

skipinit1:
  tail call void @free(i8* %alloc1) nounwind
  ret void
}

; CHECK-LABEL: @test5(
define void @test5() #1 {
  br label %bb

bb:
  tail call void @free(i8* undef) nounwind
  br label %bb
}

; CHECK-LABEL: @test6(
; CHECK: @malloc
; CHECK-NEXT: store
; CHECK-NEXT: @free
define void @test6() #1 {
  %m = call i8* @malloc(i64 24)
  store i8 0, i8* %m  ;; This is dead
  store i8 1, i8* %m
  call void @free(i8* %m)
  ret void
}

; CHECK-LABEL: @test7(
; CHECK: store
; CHECK-NEXT: store
; CHECK: ret void
define void @test7() #1 {
  %m = call i8* @malloc(i64 24)
  %m1 = getelementptr i8, i8* %m, i64 1
  %m2 = getelementptr i8, i8* %m, i64 4
  store i8 0, i8* %m1
  store i8 1, i8* %m2
  call void @free(i8* %m)
  ret void
}

; CHECK-LABEL: @test8(
; CHECK: store
; CHECK-NEXT: @free
define void @test8() #1 {
  %m = call i8* @malloc(i64 24)
  %m1 = getelementptr i8, i8* %m, i64 2
  %m2 = getelementptr i8, i8* %m, i64 2
  store i8 0, i8* %m1
  store i8 1, i8* %m2
  call void @free(i8* %m)
  ret void
}


attributes #1 = { secdse }
attributes #2 = { nounwind secdse }
