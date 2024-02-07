
%{
  char *modelname;
%}
%s model
%s amd
%s opteron
%s turion
%s athlon
%s phenom
%s ryzen
%s epyc
%s sempron
%s threadripper
%s endline
%%
<INITIAL>"model name" BEGIN(model);
<model>AMD BEGIN(amd);
<model>"Quad-Core AMD Opteron\(tm\) Processor" BEGIN(opteron);
<model>"Six-Core AMD Opteron\(tm\) Processor" BEGIN(opteron);
<model>"Mobile AMD Sempron\(tm\)"(" Processor")? BEGIN(sempron);
<model>"Athlon 64 Dual core" |
"Mobile AMD Athlon(tm) 64 Processor" |
"mobile AMD Athlon(tm) XP" |
"mobile AMD Athlon(tm) XP-M"  BEGIN(athlon);
<amd>"Embedded "(G|R)"-Series" ;
<amd>A(4|6|8|10|12)" PRO-"[^ \n\t]+ {
  printf("Basic: %s\n", yytext);
  BEGIN(endline);
}
<amd>A(4|6|8|10|12)" Extreme" {
  printf("Basic: %s\n", yytext);
  BEGIN(endline);
}
<amd>PRO ;
<amd>"Opteron\("(tm|TM)"\)"(" Processor")?  BEGIN(opteron); 
<amd>"Turion Dual-Core" |
"Turion\(tm\) 64 Mobile" |
"Turion\(tm\) 64 Mobile Technology" |
"Turion\(tm\) 64 X2 Mobile Technology" |
"Turion\(tm\) 64 X2" |
"Turion\(tm\) II"(" Neo")? |
"Turion\(tm\) II Dual-Core Mobile" |
"Turion\(tm\) II Ultra Dual-Core Mobile" |
"Turion\(tm\) Neo X2 Dual Core Processor" |
"Turion\(tm\) X2 Dual-Core Mobile" |
"Turion\(tm\)X2 Dual Core Mobile" |
"Turion\(tm\) X2 Ultra Dual-Core Mobile" |
"Turion\(tm\)X2 Ultra DualCore Mobile" BEGIN(turion);
<amd>"Athlon Dual-Core" |
"Athlon Gold" |
"Athlon Silver" |
"Athlon" |
"Athlon\(tm\)" |
"Athlon\(tm\) 64" |
"Athlon\(tm\) 64 Processor" |
"Athlon\(tm\) 64 X2 Dual Core Processor" |
"Athlon\(tm\) Dual Core Processor" |
"Athlon\(tm\) II Dual-Core" |
"Athlon\(tm\) II Neo" |
"Athlon\(tm\) II X2" |
"Athlon\(tm\) II X3" |
"Athlon\(tm\) II X4" |
"Athlon\(tm\) Neo Processor" |
"Athlon\(tm\) Neo X2 Dual Core Processor" |
"Athlon\(tm\) Processor" |
"Athlon\(tm\) X2" |
"Athlon\(tm\) X2 Dual Core Processor" |
"Athlon\(tm\) X2 Dual-Core" |
"Athlon\(tm\)X2 DualCore" |
"Athlon\(tm\) X3" |
"Athlon\(tm\) X4" |
"Athlon\(tm\) XP" |
"Athlon\(tm\) XP Processor" BEGIN(athlon);
<amd>"Phenom\(tm\)" |
"Phenom\(tm\) II"(" X"[2346])? |
"Phenom(\(tm\) X2 Dual-Core Processor" |
"Phenom(\(tm\) X4 Quad-Core Processor" BEGIN(phenom);
<amd>"Sempron\(tm\)"(" Processor"|" Dual Core Processor"|X2)? BEGIN(sempron);
<amd>"Ryzen "[3579](" PRO")? |
"Ryzen Embedded" BEGIN(ryzen);
<amd>"Ryzen Threadripper"(" PRO")? BEGIN(threadripper);
<amd>"EPYC" BEGIN(epyc);
<amd>[^ \n\t]+ {
  snprintf(modelname,256,"%s",yytext);
  //printf("Basic: %s\n", yytext);
  BEGIN(endline);
}
<sempron>[^ \n\t]+ {
  snprintf(modelname,256,"Sempron %s",yytext);
  // printf("Sempron %s\n",yytext);
  BEGIN(endline);
}
<epyc>[^ \n\t]+ {
  snprintf(modelname,256,"EPYC %s",yytext);
  // printf("EPYC %s\n", yytext);
  BEGIN(endline);
}
<threadripper>[^ \n\t]+ {
  snprintf(modelname,256,"Threadripper %s",yytext);
  // printf("Threadripper %s\n", yytext);
  BEGIN(endline);
}
<ryzen>[^ \n\t]+ {
  snprintf(modelname,256,"Ryzen %s",yytext);
  // printf("Ryzen %s\n", yytext);
  BEGIN(endline);
}
<opteron>[^ \n\t]+ {
  snprintf(modelname,256,"Opteron %s",yytext);
  // printf("opteron %s\n", modelname);
  BEGIN(endline);
}
<turion>[^ \n\t]+ {
  snprintf(modelname,256,"Turion %s",yytext);
  // printf("turion %s\n", modelname);
  BEGIN(endline);
}
<phenom>[^ \n\t]+ {
  snprintf(modelname,256,"Phenom %s",yytext);
  // printf("phenom %s\n",yytext);
  BEGIN(endline);
}
<athlon>[^ \n\t]+ {
  snprintf(modelname,256,"Athlon %s",yytext);
  // printf("athlon %s\n", modelname);
  BEGIN(endline);
}
<endline>[^\n]+ /* eat up the rest of the line*/ ;
<endline>\n BEGIN(INITIAL);
.|[ \t\n] ;
%%

int yywrap() {
  return 1;
}

char * fetch_amd_cpu_model(void){
  modelname = malloc(256);
  FILE *fp = fopen("/proc/cpuinfo","r");
  if(fp){
      yyin = fp;
  } 
  yylex();
  return modelname;
}
