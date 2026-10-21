#include <stdio.h>
#include "config.h"
#define HEADER "Mbuild\ncreated for automate and simplify builds\n"
#define USAGE "To use Mbuild first you must write a target in a form of:\n\ttarget-name:\n\t\tls\nthen run mbuild target-name then it will do that like how make file works\n"
#define DEV_INFO "Developed By: Message Beast (Melikt belay)\n"


#define VERSION_INFO "MBUILD is in version: %s\n"

void displayHelp() {
    printf("MBUILD\nVERSION: %s\n%s%s%s", __MBUILD_VERSION__, HEADER, USAGE, DEV_INFO);
}

void displayVersion() {
    printf(VERSION_INFO, __MBUILD_VERSION__);
}