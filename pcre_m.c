#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include<pcre.h>

#define VECTORSIZE (128*3)

double regex(const char* str, char* pat){
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
//  double sec_comp = 0;
//  double sec_match = 0;
  double sec_all  = 0;
  void *conf;

  conf = malloc(32);

  //  start = clock();
  re = pcre_compile(pat, 0, &errStr, &errOffset, NULL);
  if(errStr != NULL) {
    fprintf(stderr, "compile error: %s", errStr);
    return;
  }
  //  compile = clock();
//  gettimeofday(&compile_timeval, NULL);

  extra = pcre_study(re, PCRE_STUDY_JIT_COMPILE, &errStr);
  if(errStr != NULL) {
    fprintf(stderr, "study error: %s", errStr);
    return;
  }
  jit_stack = pcre_jit_stack_alloc(32*1024, 512*1024);
  pcre_assign_jit_stack(extra, NULL, jit_stack);

  //  ovector = (int*)malloc(sizeof(int)*VECTORSIZE);

  gettimeofday(&start_timeval, NULL);
  for(i = 0; i < 100; i++){

    //    match = clock();
//    gettimeofday(&match_timeval, NULL);

    if(re == NULL){
      fprintf(stderr, "matching error: %s", errStr);
      return;
    }else{
      matched = pcre_exec(re, extra, str, strlen(str), 0, 0, ovector, VECTORSIZE);
      //      matched = pcre_jit_exec(re, extra, str, (int)strlen(str), 0, 0, ovector, VECTORSIZE, jit_stack);

      if(matched >= 0) {
        printf("P, ");
      }else {
        printf("F, ");
      }
    }
    //    end = clock();

//    sec_comp += (compile_timeval.tv_sec - start_timeval.tv_sec) + (compile_timeval.tv_usec - start_timeval.tv_usec) / 1000000.0;
//    sec_match += (match_timeval.tv_sec - compile_timeval.tv_sec) + (match_timeval.tv_usec - compile_timeval.tv_usec) / 1000000.0;
//    sec_all += (end_timeval.tv_sec - start_timeval.tv_sec) + ((end_timeval.tv_usec - start_timeval.tv_usec) / 1000000.0);
    //    compiletime += (double)(compile-start)/CLOCKS_PER_SEC;
    //    matchtime += (double)(end-match)/CLOCKS_PER_SEC;
    //    alltime  += compiletime + matchtime;
  }
  printf("\n");
  gettimeofday(&end_timeval, NULL);
  sec_all += (end_timeval.tv_sec - start_timeval.tv_sec) + ((end_timeval.tv_usec - start_timeval.tv_usec) / 1000000.0);

  //  for checking jit
//  printf("pcre_version: %s\n", pcre_version());
//  pcre_config(PCRE_CONFIG_JIT, conf);
//  printf("pcre_config: %d\n", *(int *)conf);

//  printf("CompileTime Ave.: %f [us]\n", (sec_comp /  10) * 1000);
//  printf("  MatchTime Ave.: %f [us]\n", (sec_match / 10) * 1000);
//  printf("    AllTime Ave.: %f [ms]\n", (sec_all /10) * 1000);
  //  {* 100} = {/ 10 * 1000}
  //  printf("CompileTime Ave.: %f [ms]\n", compiletime * 100);
  //  printf("  MatchTime Ave.: %f [ms]\n", matchtime * 100);
  //  printf("    AllTime Ave.: %f [ms]\n", (compiletime + matchtime) * 100);

  //  free(ovector);
  pcre_free(re);
  pcre_free_study(extra);
  pcre_jit_stack_free(jit_stack);
  return sec_all/100;
}

int main(){
  double times[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int i;

  times[0] = regex("aaaaaaaaaaaac", "(?:(?:(?:a)*)*)*c");
  times[1] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaac", "a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?a?c");
  times[2] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaac", "a+a+a+a+a+a+a+a+a+a+a+a+a+a+c");
  times[3] = regex("aaaaaaaaaaaaaaaaaaaaaaac", "(?:a+)+c");
  times[4] = regex("aaaaaaaaaaaaaaaaaaaaaaac", "(?:(?:a)+)+c");
  times[5] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac", "(?:(?:aa)+)+c");
  times[6] = regex("aaaaaaaaaaaaaaaaaaaaaaac", "((a)+)+c");
  times[7] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaac", "(?:(?:(?:aa)+)+)+c");
  times[8] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaac", "(((aa)+)+)+c");
  times[9] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaac", "(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)(?:a|a)c");
  times[10] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac", "(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)(?:aa|a)c");
  times[11] = regex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac", "(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)(aa|a)c");

  for(i = 0; i < 12; i++){
    printf("%f[ms]\n", times[i]*1000);
  }
  return 0;
}
