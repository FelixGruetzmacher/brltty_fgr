/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2021 by The BRLTTY Developers.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU Lesser General Public License, as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any
 * later version. Please see the file LICENSE-LGPL for details.
 *
 * Web Page: http://brltty.app/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

#include "prologue.h"

#include "hid_defs.h"
#include "hid_tables.h"

#define HID_COLLECTION_TYPE_ENTRY(name) HID_TABLE_ENTRY(HID_COL, name)
HID_BEGIN_TABLE(CollectionType)
  HID_COLLECTION_TYPE_ENTRY(Physical),
  HID_COLLECTION_TYPE_ENTRY(Application),
  HID_COLLECTION_TYPE_ENTRY(Logical),
HID_END_TABLE(CollectionType)

#define HID_GENERIC_DESKTOP_USAGE_ENTRY(name) HID_TABLE_ENTRY(HID_USG_GDT, name)
HID_BEGIN_TABLE(GenericDesktopUsage)
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Pointer),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Mouse),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Joystick),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(GamePad),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Keyboard),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Keypad),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(MultiAxisController),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(TabletPCSystemControls),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(X),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Y),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Z),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Rx),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Ry),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Rz),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Slider),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Dial),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Wheel),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(HatSwitch),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(CountedBuffer),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(ByteCount),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(MotionWakeup),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Start),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Select),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vx),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vy),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vz),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vbrx),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vbry),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vbrz),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(Vno),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(FeatureNotification),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(ResolutionMultiplier),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemControl),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemPowerDown),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemSleep),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemWakeUp),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemContextMenu),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMainMenu),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemAppMenu),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuHelp),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuExit),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuSelect),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuRight),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuLeft),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuUp),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemMenuDown),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemColdRestart),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemWarmRestart),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(DPadUp),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(DPadDown),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(DPadRight),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(DPadLeft),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDock),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemUndock),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemSetup),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemBreak),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDebuggerBreak),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(ApplicationBreak),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(ApplicationDebuggerBreak),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemSpeakerMute),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemHibernate),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayInvert),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayInternal),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayExternal),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayBoth),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayDual),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayToggleIntExt),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplaySwap),
  HID_GENERIC_DESKTOP_USAGE_ENTRY(SystemDisplayLCDAutoscale),
HID_END_TABLE(GenericDesktopUsage)

#define HID_LEDS_USAGE_ENTRY(name) HID_TABLE_ENTRY(HID_USG_LED, name)
HID_BEGIN_TABLE(LEDsUsage)
  HID_LEDS_USAGE_ENTRY(NumLock),
  HID_LEDS_USAGE_ENTRY(CapsLock),
  HID_LEDS_USAGE_ENTRY(ScrollLock),
  HID_LEDS_USAGE_ENTRY(Compose),
  HID_LEDS_USAGE_ENTRY(Kana),
  HID_LEDS_USAGE_ENTRY(Power),
  HID_LEDS_USAGE_ENTRY(Shift),
  HID_LEDS_USAGE_ENTRY(DoNotDisturb),
  HID_LEDS_USAGE_ENTRY(Mute),
  HID_LEDS_USAGE_ENTRY(ToneEnable),
  HID_LEDS_USAGE_ENTRY(HighCutFilter),
  HID_LEDS_USAGE_ENTRY(LowCutFilter),
  HID_LEDS_USAGE_ENTRY(EqualizerEnable),
  HID_LEDS_USAGE_ENTRY(SoundFieldOn),
  HID_LEDS_USAGE_ENTRY(SurroundOn),
  HID_LEDS_USAGE_ENTRY(Repeat),
  HID_LEDS_USAGE_ENTRY(Stereo),
  HID_LEDS_USAGE_ENTRY(SamplingRateDetect),
  HID_LEDS_USAGE_ENTRY(Spinning),
  HID_LEDS_USAGE_ENTRY(CAV),
  HID_LEDS_USAGE_ENTRY(CLV),
  HID_LEDS_USAGE_ENTRY(RecordingFormatDetect),
  HID_LEDS_USAGE_ENTRY(OffHook),
  HID_LEDS_USAGE_ENTRY(Ring),
  HID_LEDS_USAGE_ENTRY(MessageWaiting),
  HID_LEDS_USAGE_ENTRY(DataMode),
  HID_LEDS_USAGE_ENTRY(BatteryOperation),
  HID_LEDS_USAGE_ENTRY(BatteryOK),
  HID_LEDS_USAGE_ENTRY(BatteryLow),
  HID_LEDS_USAGE_ENTRY(Speaker),
  HID_LEDS_USAGE_ENTRY(HeadSet),
  HID_LEDS_USAGE_ENTRY(Hold),
  HID_LEDS_USAGE_ENTRY(Microphone),
  HID_LEDS_USAGE_ENTRY(Coverage),
  HID_LEDS_USAGE_ENTRY(NightMode),
  HID_LEDS_USAGE_ENTRY(SendCalls),
  HID_LEDS_USAGE_ENTRY(CallPickup),
  HID_LEDS_USAGE_ENTRY(Conference),
  HID_LEDS_USAGE_ENTRY(StandBy),
  HID_LEDS_USAGE_ENTRY(CameraOn),
  HID_LEDS_USAGE_ENTRY(CameraOff),
  HID_LEDS_USAGE_ENTRY(OnLine),
  HID_LEDS_USAGE_ENTRY(OffLine),
  HID_LEDS_USAGE_ENTRY(Busy),
  HID_LEDS_USAGE_ENTRY(Ready),
  HID_LEDS_USAGE_ENTRY(PaperOut),
  HID_LEDS_USAGE_ENTRY(PaperJam),
  HID_LEDS_USAGE_ENTRY(Remote),
  HID_LEDS_USAGE_ENTRY(Forward),
  HID_LEDS_USAGE_ENTRY(Reverse),
  HID_LEDS_USAGE_ENTRY(Stop),
  HID_LEDS_USAGE_ENTRY(Rewind),
  HID_LEDS_USAGE_ENTRY(FastForward),
  HID_LEDS_USAGE_ENTRY(Play),
  HID_LEDS_USAGE_ENTRY(Pause),
  HID_LEDS_USAGE_ENTRY(Record),
  HID_LEDS_USAGE_ENTRY(Error),
  HID_LEDS_USAGE_ENTRY(UsageSelectedIndicator),
  HID_LEDS_USAGE_ENTRY(UsageInUseIndicator),
  HID_LEDS_USAGE_ENTRY(UsageMultiModeIndicator),
  HID_LEDS_USAGE_ENTRY(IndicatorOn),
  HID_LEDS_USAGE_ENTRY(IndicatorFlash),
  HID_LEDS_USAGE_ENTRY(IndicatorSlowBlink),
  HID_LEDS_USAGE_ENTRY(IndicatorFastBlink),
  HID_LEDS_USAGE_ENTRY(IndicatorOff),
  HID_LEDS_USAGE_ENTRY(FlashOnTime),
  HID_LEDS_USAGE_ENTRY(SlowBlinkOnTime),
  HID_LEDS_USAGE_ENTRY(SlowBlinkOffTime),
  HID_LEDS_USAGE_ENTRY(FastBlinkOnTime),
  HID_LEDS_USAGE_ENTRY(FastBlinkOffTime),
  HID_LEDS_USAGE_ENTRY(UsageIndicatorColor),
  HID_LEDS_USAGE_ENTRY(IndicatorRed),
  HID_LEDS_USAGE_ENTRY(IndicatorGreen),
  HID_LEDS_USAGE_ENTRY(IndicatorAmber),
  HID_LEDS_USAGE_ENTRY(GenericIndicator),
  HID_LEDS_USAGE_ENTRY(SystemSuspend),
  HID_LEDS_USAGE_ENTRY(ExternalPowerConnected),
HID_END_TABLE(LEDsUsage)

#define HID_USAGE_PAGE_ENTRY(name, usages) HID_TABLE_ENTRY(HID_UPG, name, .usageTable=usages)
HID_BEGIN_TABLE(UsagePage)
  HID_USAGE_PAGE_ENTRY(GenericDesktop, &hidGenericDesktopUsageTable),
  HID_USAGE_PAGE_ENTRY(Simulation, NULL),
  HID_USAGE_PAGE_ENTRY(VirtualReality, NULL),
  HID_USAGE_PAGE_ENTRY(Sport, NULL),
  HID_USAGE_PAGE_ENTRY(Game, NULL),
  HID_USAGE_PAGE_ENTRY(GenericDevice, NULL),
  HID_USAGE_PAGE_ENTRY(KeyboardKeypad, NULL),
  HID_USAGE_PAGE_ENTRY(LEDs, &hidLEDsUsageTable),
  HID_USAGE_PAGE_ENTRY(Button, NULL),
  HID_USAGE_PAGE_ENTRY(Ordinal, NULL),
  HID_USAGE_PAGE_ENTRY(Telephony, NULL),
  HID_USAGE_PAGE_ENTRY(Consumer, NULL),
  HID_USAGE_PAGE_ENTRY(Digitizer, NULL),
  HID_USAGE_PAGE_ENTRY(PhysicalInterfaceDevice, NULL),
  HID_USAGE_PAGE_ENTRY(Unicode, NULL),
  HID_USAGE_PAGE_ENTRY(AlphanumericDisplay, NULL),
  HID_USAGE_PAGE_ENTRY(MedicalInstruments, NULL),
  HID_USAGE_PAGE_ENTRY(BarCodeScanner, NULL),
  HID_USAGE_PAGE_ENTRY(Braille, NULL),
  HID_USAGE_PAGE_ENTRY(Scale, NULL),
  HID_USAGE_PAGE_ENTRY(MagneticStripeReader, NULL),
  HID_USAGE_PAGE_ENTRY(Camera, NULL),
  HID_USAGE_PAGE_ENTRY(Arcade, NULL),
HID_END_TABLE(UsagePage)
