#ifndef _GESTURE_INTERACTION_H_
#define _GESTURE_INTERACTION_H_

// Gesture interface for all platform.Platform code should analysis
// the gesture first, then pass result inside.

void onKeyPressAt(float x, float y);
void onKeyUnPress();
void onScroll(float dx, float dy);
bool onZoom(float percentX, float percentY);

#endif /* _GESTURE_INTERACTION_H_ */
