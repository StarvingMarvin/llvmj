

%MJArray = type { i8*, i8*, i32 }

declare i8* @malloc(i64)
declare i32 @puts(i8*)

@mj.numberFormat = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str1 = private unnamed_addr constant [5 x i8] c"%d, \00", align 1

define i32 @len(%MJArray* nocapture byval %arr) {
  %endPtr = getelementptr inbounds %MJArray* %arr, i64 0, i32 1
  %end = load i8** %endPtr
  %beginPtr = getelementptr inbounds %MJArray* %arr, i64 0, i32 0
  %begin = load i8** %beginPtr
  %endInt = ptrtoint i8* %end to i64
  %beginInt = ptrtoint i8* %begin to i64
  %diff = sub i64 %endInt, %beginInt

  %elemSizePtr = getelementptr inbounds %MJArray* %arr, i64 0, i32 2
  %elemSize = load i32* %elemSizePtr
  %elemSize = sext i32 %elemSize to i64
  %div = sdiv i64 %diff, %elemSize
  %div = trunc i64 %div to i32
  ret i32 %div
}

%List = type { i32, %List* }

@inputNumbersStr = global %MJArray*
@sortedListStr = global %MJArray*

@MAX_SIZE = constant i32 100

@list = global %List*

define %List* @cons(i32 %val, %List* %list) {
    %1 = call i8* @malloc(i64 16)
    %newHead = bitcast i8* %1 to %List*
    %valPtr = bitcast i8* %1 to i32*
    store i32 %val, i32* %valPtr
    %tailPtr = getelementptr %List** %newHead, i32 0, i32 1
    store %List* %list, %List** %tailPtr
    ret %List* %newHead
}

define void @insertAfter(%List* %list, i32 %val) {
    %1 = call i8* @malloc(i64 16)
    %newElem = bitcast i8* %1 to %List*
    %valPtr = bitcast i8* %1 to i32*
    store i32 %val, i32* %valPtr

    %tailPtr = getelementptr %List** %list, i32 0, i32 1
    %tail = load %List** %tailPtr

    %newTailPtr = getelementptr %List** %newElem, i32 0, i32 1
    store %List* %tail, %List** %newTailPtr
    store %List* %newElem, %List** %tailPtr

    ret void
}

;define %List* @readList() {
  ;%v = alloca i32, align 4
  ;%1 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i64 0, i64 0), i32* %v) nounwind
  ;%2 = load i32* %v, align 4, !tbaa !3
  ;%3 = icmp slt i32 %2, 0
  ;br i1 %3, label %.loopexit, label %4
;
;; <label>:4                                       ; preds = %0
  ;%5 = call noalias i8* @malloc(i64 16) nounwind
  ;%6 = bitcast i8* %5 to %struct.LL*
  ;%7 = bitcast i8* %5 to i32*
  ;store i32 %2, i32* %7, align 4, !tbaa !3
  ;%8 = getelementptr inbounds i8* %5, i64 8
  ;%9 = bitcast i8* %8 to %struct.LL**
  ;store %struct.LL* null, %struct.LL** %9, align 8, !tbaa !0
  ;%10 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i64 0, i64 0), i32* %v) nounwind
  ;%11 = load i32* %v, align 4, !tbaa !3
  ;%12 = icmp slt i32 %11, 0
  ;br i1 %12, label %.loopexit, label %.lr.ph
;
;.lr.ph:                                           ; preds = %30, %4
  ;%13 = phi i32 [ %37, %30 ], [ %11, %4 ]
  ;%head.02 = phi %struct.LL* [ %head.1, %30 ], [ %6, %4 ]
  ;%14 = getelementptr inbounds %struct.LL* %head.02, i64 0, i32 0
  ;%15 = load i32* %14, align 4, !tbaa !3
  ;%16 = icmp slt i32 %13, %15
  ;br i1 %16, label %17, label %23
;
;; <label>:17                                      ; preds = %.lr.ph
  ;%18 = call noalias i8* @malloc(i64 16) nounwind
  ;%19 = bitcast i8* %18 to %struct.LL*
  ;%20 = bitcast i8* %18 to i32*
  ;store i32 %13, i32* %20, align 4, !tbaa !3
  ;%21 = getelementptr inbounds i8* %18, i64 8
  ;%22 = bitcast i8* %21 to %struct.LL**
  ;store %struct.LL* %head.02, %struct.LL** %22, align 8, !tbaa !0
  ;br label %23
;
;; <label>:23                                      ; preds = %17, %.lr.ph
  ;%head.1 = phi %struct.LL* [ %19, %17 ], [ %head.02, %.lr.ph ]
  ;br label %24
;
;; <label>:24                                      ; preds = %24, %23
  ;%cur.0 = phi %struct.LL* [ %head.1, %23 ], [ %26, %24 ]
  ;%25 = getelementptr inbounds %struct.LL* %cur.0, i64 0, i32 1
  ;%26 = load %struct.LL** %25, align 8, !tbaa !0
  ;%27 = getelementptr inbounds %struct.LL* %26, i64 0, i32 0
  ;%28 = load i32* %27, align 4, !tbaa !3
  ;%29 = icmp sgt i32 %13, %28
  ;br i1 %29, label %24, label %30
;
;; <label>:30                                      ; preds = %24
  ;%31 = call noalias i8* @malloc(i64 16) nounwind
  ;%32 = bitcast i8* %31 to %struct.LL*
  ;%33 = bitcast i8* %31 to i32*
  ;store i32 %13, i32* %33, align 4, !tbaa !3
  ;%34 = getelementptr inbounds i8* %31, i64 8
  ;%35 = bitcast i8* %34 to %struct.LL**
  ;store %struct.LL* %26, %struct.LL** %35, align 8, !tbaa !0
  ;store %struct.LL* %32, %struct.LL** %25, align 8, !tbaa !0
  ;%36 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i64 0, i64 0), i32* %v) nounwind
  ;%37 = load i32* %v, align 4, !tbaa !3
  ;%38 = icmp slt i32 %37, 0
  ;br i1 %38, label %.loopexit, label %.lr.ph
;
;.loopexit:                                        ; preds = %30, %4, %0
  ;%.0 = phi %struct.LL* [ null, %0 ], [ %6, %4 ], [ %head.1, %30 ]
  ;ret %struct.LL* %.0
;}
;
;declare i32 @__isoc99_scanf(i8*, ...)
;
;define void @writeList(%struct.LL* %list) nounwind uwtable {
  ;%1 = icmp eq %struct.LL* %list, null
  ;br i1 %1, label %6, label %.lr.ph..lr.ph.split_crit_edge
;
;.lr.ph..lr.ph.split_crit_edge:                    ; preds = %0
  ;%2 = getelementptr inbounds %struct.LL* %list, i64 0, i32 0
  ;br label %3
;
;; <label>:3                                       ; preds = %3, %.lr.ph..lr.ph.split_crit_edge
  ;%4 = load i32* %2, align 4, !tbaa !3
  ;%5 = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @.str1, i64 0, i64 0), i32 %4) nounwind
  ;br label %3
;
;; <label>:6                                       ; preds = %0
  ;%putchar = tail call i32 @putchar(i32 10) nounwind
  ;ret void
;}

