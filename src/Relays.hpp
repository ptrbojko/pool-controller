#pragma once

void setupRelays();
void handleRelays();

void switchRelay1(boolean mode);
void switchRelay2(boolean mode);

void lockRelay1(boolean mode, boolean lock);
void lockRelay2(boolean mode, boolean lock);