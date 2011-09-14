; ModuleID = '/tmp/webcompile/_12570_0.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-unknown-linux-gnu"

%struct.MJArray = type { i8*, i8*, i32 }

@.str = private constant [4 x i8] c"a: \00"       ; <[4 x i8]*> [#uses=1]
@.str1 = private constant [5 x i8] c"90: \00"     ; <[5 x i8]*> [#uses=1]
@.str2 = private constant [7 x i8] c"size: \00"   ; <[7 x i8]*> [#uses=1]

define i32 @ord(i8 signext %c) nounwind readnone {
entry:
  %conv = sext i8 %c to i32                       ; <i32> [#uses=1]
  ret i32 %conv
}

define i32 @chr(i32 %i) nounwind readnone {
entry:
  %sext = shl i32 %i, 24                          ; <i32> [#uses=1]
  %conv1 = ashr i32 %sext, 24                     ; <i32> [#uses=1]
  ret i32 %conv1
}

define i32 @len(%struct.MJArray* nocapture byval %arr) nounwind readonly {
entry:
  %tmp = getelementptr inbounds %struct.MJArray* %arr, i64 0, i32 1 ; <i8**> [#uses=1]
  %tmp1 = load i8** %tmp                          ; <i8*> [#uses=1]
  %tmp2 = getelementptr inbounds %struct.MJArray* %arr, i64 0, i32 0 ; <i8**> [#uses=1]
  %tmp3 = load i8** %tmp2                         ; <i8*> [#uses=1]
  %sub.ptr.lhs.cast = ptrtoint i8* %tmp1 to i64   ; <i64> [#uses=1]
  %sub.ptr.rhs.cast = ptrtoint i8* %tmp3 to i64   ; <i64> [#uses=1]
  %sub.ptr.sub = sub i64 %sub.ptr.lhs.cast, %sub.ptr.rhs.cast ; <i64> [#uses=1]
  %tmp4 = getelementptr inbounds %struct.MJArray* %arr, i64 0, i32 2 ; <i32*> [#uses=1]
  %tmp5 = load i32* %tmp4                         ; <i32> [#uses=1]
  %conv = sext i32 %tmp5 to i64                   ; <i64> [#uses=1]
  %div = sdiv i64 %sub.ptr.sub, %conv             ; <i64> [#uses=1]
  %conv6 = trunc i64 %div to i32                  ; <i32> [#uses=1]
  ret i32 %conv6
}

define void @mj_main() nounwind {
entry:
  %call1 = tail call i32 @puts(i8* getelementptr ([4 x i8]* @.str, i64 24, i64 1)) nounwind ; <i32> [#uses=0]
  %call5 = tail call i32 @puts(i8* getelementptr ([5 x i8]* @.str1, i64 18, i64 0)) nounwind ; <i32> [#uses=0]
  %call16 = tail call i32 @puts(i8* getelementptr inbounds ([7 x i8]* @.str2, i64 0, i64 4)) nounwind ; <i32> [#uses=0]
  ret void
}

declare i32 @puts(i8* nocapture) nounwind

define i32 @main(i32 %argc, i8** nocapture %argv) nounwind {
entry:
  %call1.i = tail call i32 @puts(i8* getelementptr ([4 x i8]* @.str, i64 24, i64 1)) nounwind ; <i32> [#uses=0]
  %call5.i = tail call i32 @puts(i8* getelementptr ([5 x i8]* @.str1, i64 18, i64 0)) nounwind ; <i32> [#uses=0]
  %call16.i = tail call i32 @puts(i8* getelementptr inbounds ([7 x i8]* @.str2, i64 0, i64 4)) nounwind ; <i32> [#uses=0]
  ret i32 0
}

