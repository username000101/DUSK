#pragma once
#if defined(_WIN32)
#define FUNCSIG __FUNCSIG__
#define DUSK_EXPORT __declspec(dllexport)
#else
#define FUNCSIG __PRETTY_FUNCTION__
#define LIT_EXPORT __attribute__((visibility("default")))
#endif

#define DUSK_LOGO \
  "DDDDDDDDDDDDD      UUUUUUU       UUUUUUUU   SSSSSSSSSSSSSSS KKKKKKKKK    KKKKKKK\n"\
  "D::::::::::::DDD   U:::::U       U::::::U SS:::::::::::::::SK:::::::K    K:::::K\n"\
  "D:::::::::::::::DD U:::::U       U::::::US:::::SSSSSS::::::SK:::::::K    K:::::K\n"\
  "D::::::DDDDDD:::::DU:::::U       U:::::UUS:::::S     SSSSSSSK:::::::K   K::::::K\n"\
  "D:::::D     D:::::DU:::::U       U:::::U S:::::S            KK::::::K  K:::::KKK\n"\
  "D:::::D     D:::::DU:::::D       D:::::U S:::::S              K:::::K K:::::K\n"\
  "D:::::D     D:::::DU:::::D       D:::::U  S::::SSSS           K::::::K:::::K\n"\
  "D:::::D     D:::::DU:::::D       D:::::U   SS::::::SSSSS      K:::::::::::K\n"\
  "D:::::D     D:::::DU:::::D       D:::::U     SSS::::::::SS    K:::::::::::K\n"\
  "D:::::D     D:::::DU:::::D       D:::::U        SSSSSS::::S   K::::::K:::::K\n"\
  "D:::::D     D:::::DU:::::D       D:::::U             S:::::S  K:::::K K:::::K\n"\
  "D:::::D     D:::::D U::::::U     U:::::U              S:::::SKK::::::K  K:::::KK\n"\
  "D:::::DDDDDDD:::::D  U:::::::UUUU:::::U SSSSSSS     S:::::SK:::::::K   K::::::K\n"\
  "D:::::::::::::::DD    UU:::::::::::::UU  S::::::SSSSSS:::::SK:::::::K    K:::::K\n"\
  "D::::::::::::DDD        UU:::::::::UU    S:::::::::::::::SS K:::::::K    K:::::K\n"\
  "DDDDDDDDDDDDD             UUUUUUUUU       SSSSSSSSSSSSSSS   KKKKKKKKK    KKKKKKK"