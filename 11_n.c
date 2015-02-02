#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<pcre.h>

clock_t start, mid, end;

int match(const char* str, char* pat){
  pcre *re;
  const char* errStr;
  int errOffset;
  int matched;
  int ovector = 0;
  pcre_extra *extra;
  pcre_jit_stack *jit_stack;

  re = pcre_compile(pat, 0, &errStr, &errOffset, NULL);
  mid = clock();

  extra = pcre_study(re, PCRE_STUDY_JIT_COMPILE, &errStr);
  jit_stack = pcre_jit_stack_alloc(32*1024, 512*1024);
  pcre_assign_jit_stack(extra, NULL, jit_stack);

  if(re == NULL){
    fprintf(stderr, "matching error: %s", errStr);
    return -1;
  }else{
    pcre_assign_jit_stack(extra, NULL, jit_stack);
    matched = pcre_exec(re, extra, str, (int)strlen(str), 0, 0, &ovector, 30);
    pcre_free(re);
    pcre_free_study(extra);
    pcre_jit_stack_free(jit_stack);
    if(matched < 0) return 1;
    else return 0;
  }

}

int main(int argc, char* argv[]){
  int result;
  double compiletime, matchtime;

  start = clock();
  result = match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)c");
  end = clock();

  if( result == 0) printf("matched\n");
  else printf("unmatched\n");

  compiletime = (double)(mid-start)/CLOCKS_PER_SEC;
  matchtime = (double)(end-mid)/CLOCKS_PER_SEC;
  printf("CompileTime: %f [ms]\n", compiletime * 1000);
  printf("  MatchTime: %f [ms]\n", matchtime * 1000);
  printf("    AllTime: %f [ms]\n", (compiletime + matchtime) * 1000);

  return 0;
}
