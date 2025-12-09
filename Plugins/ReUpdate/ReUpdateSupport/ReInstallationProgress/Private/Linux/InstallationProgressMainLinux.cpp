#include "ReInstallationProgress.h"
#include "UnixCommonStartup.h"

int main(int argc, char* argv[])
{
	return CommonUnixMain(argc, argv, &RunReInstallationProgress);
}