
#ifndef SAMPLEFRAMEWORK_H_
#define SAMPLEFRAMEWORK_H_


void SfwOpenWindow(const char* title);
void SfwCloseWindow();
void SfwUpdate();
bool SfwIsDone();

void SfwSetInputManager(gainput::InputManager* manager);

int SfwGetWidth();
int SfwGetHeight();


#if defined(GAINPUT_PLATFORM_LINUX)
Display* SfwGetXDisplay();
#endif

#endif

