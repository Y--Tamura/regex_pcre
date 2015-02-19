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

      printf("%d, ", matched);
      if(matched >= 0) {
        printf("matched\n");
      }else {
        printf("unmatched\n");
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
  int num=0;
  printf("num:");
  scanf("%d", &num);
  switch(num){
  case 1:
  regex("aaaaaaaaaaaabc", "(?:(?:(?:a)*)*)*c");
  break;
  case 2:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaabc", "a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?c");
  break;
  case 3:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbc", "a+a+a+a+a+a+a+a+a+a+a+a+a+a+c");
  break;
  case 4:
  regex("aaaaaaaaaaaaaaaaaaaaaaabc", "(?:a+)+c");
  break;
  case 5:
  regex("aaaaaaaaaaaaaaaaaaaaaaabc", "(?:(?:a)+)+c");
  break;
  case 6:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:(?:aa)+)+c");
  break;
  case 7:
  regex("aaaaaaaaaaaaaaaaaaaaaaabc", "((a)+)+c");
  break;
  case 8:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:(?:(?:aa)+)+)+c");
  break;
  case 9:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(((aa)+)+)+c");
  break;
  case 10:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)c");
  break;
  case 11:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)c");
  break;
  case 12:
  regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabc", "(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)c");
  break;
  default:
  printf("1-12¥n");
  }
  return 0;
}
