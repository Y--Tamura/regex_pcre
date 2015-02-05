#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include<pcre.h>

#define VECTORSIZE (128*3)

void regex(const char* str, char* pat){
  double compiletime = 0;
  double matchtime = 0;
  double alltime = 0;
  int i;
  pcre *re;
  const char* errStr;
  int errOffset;
  int matched;
  //  int *ovector;
  int ovector[VECTORSIZE];
  pcre_extra *extra;
  pcre_jit_stack *jit_stack;
  //  clock_t start, compile, match, end;
  struct timeval start_timeval, compile_timeval , match_timeval , end_timeval;
  double sec_comp = 0;
  double sec_match = 0;
  double sec_all  = 0;
  void *conf;

  conf = malloc(32);

  //  start = clock();
  gettimeofday(&start_timeval, NULL);
  re = pcre_compile(pat, 0, &errStr, &errOffset, NULL);
  if(errStr != NULL) {
    fprintf(stderr, "compile error: %s", errStr);
    return;
  }
  //  compile = clock();
  gettimeofday(&compile_timeval, NULL);

  extra = pcre_study(re, PCRE_STUDY_JIT_COMPILE, &errStr);
  if(errStr != NULL) {
    fprintf(stderr, "study error: %s", errStr);
    return;
  }
  jit_stack = pcre_jit_stack_alloc(32*1024, 512*1024);
  pcre_assign_jit_stack(extra, NULL, jit_stack);

  //  ovector = (int*)malloc(sizeof(int)*VECTORSIZE);

  for(i = 0; i < 10; i++){

    //    match = clock();
    gettimeofday(&match_timeval, NULL);

    if(re == NULL){
      fprintf(stderr, "matching error: %s", errStr);
      return;
    }else{
      matched = pcre_exec(re, extra, str, strlen(str), 0, 0, ovector, VECTORSIZE);
      //      matched = pcre_jit_exec(re, extra, str, (int)strlen(str), 0, 0, ovector, VECTORSIZE, jit_stack);

      printf("%d\n", matched);
      if(matched >= 0) {
        printf("matched, ");
      }else {
        printf("unmatched, ");
      }
    }
    //    end = clock();
    gettimeofday(&end_timeval, NULL);

    sec_comp += (compile_timeval.tv_sec - start_timeval.tv_sec) + (compile_timeval.tv_usec - start_timeval.tv_usec) / 1000000.0;
    sec_match += (match_timeval.tv_sec - compile_timeval.tv_sec) + (match_timeval.tv_usec - compile_timeval.tv_usec) / 1000000.0;
    sec_all += ((compile_timeval.tv_sec - start_timeval.tv_sec) + (compile_timeval.tv_usec - start_timeval.tv_usec) / 1000000.0
                + (match_timeval.tv_sec - compile_timeval.tv_sec) + (match_timeval.tv_usec - compile_timeval.tv_usec) / 1000000.0);
    //    compiletime += (double)(compile-start)/CLOCKS_PER_SEC;
    //    matchtime += (double)(end-match)/CLOCKS_PER_SEC;
    //    alltime  += compiletime + matchtime;
  }
  printf("\n");

  //  for checking jit
  printf("pcre_version: %s\n", pcre_version());
  pcre_config(PCRE_CONFIG_JIT, conf);
  printf("pcre_config: %d\n", *(int *)conf);

  printf("CompileTime Ave.: %f [us]\n", (sec_comp /  10) * 1000);
  printf("  MatchTime Ave.: %f [us]\n", (sec_match / 10) * 1000);
  printf("    AllTime Ave.: %f [us]\n", (sec_all / 10) * 1000);
  //  {* 100} = {/ 10 * 1000}
  //  printf("CompileTime Ave.: %f [ms]\n", compiletime * 100);
  //  printf("  MatchTime Ave.: %f [ms]\n", matchtime * 100);
  //  printf("    AllTime Ave.: %f [ms]\n", (compiletime + matchtime) * 100);

  //  free(ovector);
  pcre_free(re);
  pcre_free_study(extra);
  pcre_jit_stack_free(jit_stack);
}

int main(){
  printf("01\n");
  regex("aaaaaaaaaaaabc", "(?:(?:(?:a)*)*)*c");
  printf("02\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaabc", "a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?c");
  printf("03\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbc", "a+a+a+a+a+a+a+a+a+a+a+a+a+a+c");
  printf("04\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaabc", "(?:a+)+c");
  printf("05\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaabc", "(?:(?:a)+)+c");
  printf("06\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:(?:aa)+)+c");
  printf("07\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaabc", "((a)+)+c");
  printf("08\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:(?:(?:aa)+)+)+c");
  printf("09\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(((aa)+)+)+c");
  printf("10\n");
#if 0
  static char regex_pattern[1024] = {'\0'};
  static char input[28] = {};
  static char origin[] = "(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)c";
  char *p = regex_pattern;
  int i = 0;
  for (i = 0; i < 26; i++) {
    const char *s = "(?:a|a)";
    memcpy(p, s, strlen(s));
    input[i] = 'a';
    p +=strlen(s);
  }
  p[0] = 'c';
  input[26] = 'c';
  printf("eq=%d\n", strcmp(regex_pattern, origin));
  printf("%s\n%s\n", regex_pattern, origin);
  regex("12345", "/[1-5]*/");
  regex(input, regex_pattern);
  #endif
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)c");
  printf("11\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)c");
  printf("12\n");
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)c");
  return 0;
}
