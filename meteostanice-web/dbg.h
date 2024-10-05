#if DEBUG

#define Dprint(n) Serial.print(n)
#define Dprintln(n) Serial.println(n)
#define Dprintln2(n,m) Serial.println(n,m)

#else

#define Dprint(n)
#define Dprintln(n)
#define Dprintln2(n,m)

#endif