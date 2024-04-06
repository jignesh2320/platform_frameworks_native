/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#undef LOG_TAG
#define LOG_TAG "LibSurfaceFlingerUnittests"

#include "DisplayTransactionTestHelpers.h"
#include "mock/MockFrameRateMode.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define EXPECT_DISPLAY_MODE_REQUEST(expected, requestOpt)                               \
    ASSERT_TRUE(requestOpt);                                                            \
    EXPECT_FRAME_RATE_MODE(expected.mode.modePtr, expected.mode.fps, requestOpt->mode); \
    EXPECT_EQ(expected.emitEvent, requestOpt->emitEvent)

namespace android {
namespace {

using FakeDisplayDeviceInjector = TestableSurfaceFlinger::FakeDisplayDeviceInjector;
using DisplayModeRequest = display::DisplayModeRequest;

class InitiateModeChangeTest : public DisplayTransactionTest {
public:
    using Action = DisplayDevice::DesiredModeAction;
    void SetUp() override {
        injectFakeBufferQueueFactory();
        injectFakeNativeWindowSurfaceFactory();

        PrimaryDisplayVariant::setupHwcHotplugCallExpectations(this);
        PrimaryDisplayVariant::setupFramebufferConsumerBufferQueueCallExpectations(this);
        PrimaryDisplayVariant::setupFramebufferProducerBufferQueueCallExpectations(this);
        PrimaryDisplayVariant::setupNativeWindowSurfaceCreationCallExpectations(this);
        PrimaryDisplayVariant::setupHwcGetActiveConfigCallExpectations(this);

        mFlinger.onComposerHalHotplugEvent(PrimaryDisplayVariant::HWC_DISPLAY_ID,
                                           DisplayHotplugEvent::CONNECTED);
        mFlinger.configureAndCommit();

        mDisplay = PrimaryDisplayVariant::makeFakeExistingDisplayInjector(this)
                           .setDisplayModes(makeModes(kMode60, kMode90, kMode120), kModeId60)
                           .inject();
    }

protected:
    sp<DisplayDevice> mDisplay;

    static constexpr DisplayModeId kModeId60{0};
    static constexpr DisplayModeId kModeId90{1};
    static constexpr DisplayModeId kModeId120{2};

    static inline const ftl::NonNull<DisplayModePtr> kMode60 =
            ftl::as_non_null(createDisplayMode(kModeId60, 60_Hz));
    static inline const ftl::NonNull<DisplayModePtr> kMode90 =
            ftl::as_non_null(createDisplayMode(kModeId90, 90_Hz));
    static inline const ftl::NonNull<DisplayModePtr> kMode120 =
            ftl::as_non_null(createDisplayMode(kModeId120, 120_Hz));

    static inline const DisplayModeRequest kDesiredMode30{{30_Hz, kMode60}, .emitEvent = false};
    static inline const DisplayModeRequest kDesiredMode60{{60_Hz, kMode60}, .emitEvent = true};
    static inline const DisplayModeRequest kDesiredMode90{{90_Hz, kMode90}, .emitEvent = false};
    static inline const DisplayModeRequest kDesiredMode120{{120_Hz, kMode120}, .emitEvent = true};
};

TEST_F(InitiateModeChangeTest, setDesiredModeToActiveMode) {
    EXPECT_EQ(Action::None, mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode60)));
    EXPECT_FALSE(mDisplay->getDesiredMode());
}

TEST_F(InitiateModeChangeTest, setDesiredMode) {
    EXPECT_EQ(Action::InitiateDisplayModeSwitch,
<<<<<<< HEAD
              mDisplay->setDesiredActiveMode(
                      {scheduler::FrameRateMode{90_Hz, kMode90}, Event::None}));
    ASSERT_NE(std::nullopt, mDisplay->getDesiredActiveMode());
    EXPECT_FRAME_RATE_MODE(kMode90, 90_Hz, *mDisplay->getDesiredActiveMode()->modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getDesiredActiveMode()->event);

    // Setting another mode should be cached but return None
    EXPECT_EQ(Action::None,
              mDisplay->setDesiredActiveMode(
                      {scheduler::FrameRateMode{120_Hz, kMode120}, Event::None}));
    ASSERT_NE(std::nullopt, mDisplay->getDesiredActiveMode());
    EXPECT_FRAME_RATE_MODE(kMode120, 120_Hz, *mDisplay->getDesiredActiveMode()->modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getDesiredActiveMode()->event);
=======
              mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode90)));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode90, mDisplay->getDesiredMode());

    EXPECT_EQ(Action::None, mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode120)));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode120, mDisplay->getDesiredMode());
>>>>>>> android-14.0.0_r31
}

TEST_F(InitiateModeChangeTest, clearDesiredMode) {
    EXPECT_EQ(Action::InitiateDisplayModeSwitch,
              mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode90)));
    EXPECT_TRUE(mDisplay->getDesiredMode());

    mDisplay->clearDesiredMode();
    EXPECT_FALSE(mDisplay->getDesiredMode());
}

TEST_F(InitiateModeChangeTest, initiateModeChange) REQUIRES(kMainThreadContext) {
    EXPECT_EQ(Action::InitiateDisplayModeSwitch,
<<<<<<< HEAD
              mDisplay->setDesiredActiveMode(
                      {scheduler::FrameRateMode{90_Hz, kMode90}, Event::None}));
    ASSERT_NE(std::nullopt, mDisplay->getDesiredActiveMode());
    EXPECT_FRAME_RATE_MODE(kMode90, 90_Hz, *mDisplay->getDesiredActiveMode()->modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getDesiredActiveMode()->event);
=======
              mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode90)));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode90, mDisplay->getDesiredMode());
>>>>>>> android-14.0.0_r31

    const hal::VsyncPeriodChangeConstraints constraints{
            .desiredTimeNanos = systemTime(),
            .seamlessRequired = false,
    };
    hal::VsyncPeriodChangeTimeline timeline;
    EXPECT_TRUE(mDisplay->initiateModeChange(*mDisplay->getDesiredMode(), constraints, timeline));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode90, mDisplay->getPendingMode());

    mDisplay->clearDesiredMode();
    EXPECT_FALSE(mDisplay->getDesiredMode());
}

TEST_F(InitiateModeChangeTest, initiateRenderRateSwitch) {
    EXPECT_EQ(Action::InitiateRenderRateSwitch,
              mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode30)));
    EXPECT_FALSE(mDisplay->getDesiredMode());
}

TEST_F(InitiateModeChangeTest, initiateDisplayModeSwitch) FTL_FAKE_GUARD(kMainThreadContext) {
    EXPECT_EQ(Action::InitiateDisplayModeSwitch,
<<<<<<< HEAD
              mDisplay->setDesiredActiveMode(
                      {scheduler::FrameRateMode{90_Hz, kMode90}, Event::None}));
    ASSERT_NE(std::nullopt, mDisplay->getDesiredActiveMode());
    EXPECT_FRAME_RATE_MODE(kMode90, 90_Hz, *mDisplay->getDesiredActiveMode()->modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getDesiredActiveMode()->event);
=======
              mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode90)));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode90, mDisplay->getDesiredMode());
>>>>>>> android-14.0.0_r31

    const hal::VsyncPeriodChangeConstraints constraints{
            .desiredTimeNanos = systemTime(),
            .seamlessRequired = false,
    };
    hal::VsyncPeriodChangeTimeline timeline;
<<<<<<< HEAD
    EXPECT_EQ(OK,
              mDisplay->initiateModeChange(*mDisplay->getDesiredActiveMode(), constraints,
                                           &timeline));
    EXPECT_FRAME_RATE_MODE(kMode90, 90_Hz, *mDisplay->getUpcomingActiveMode().modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getUpcomingActiveMode().event);

    EXPECT_EQ(Action::None,
              mDisplay->setDesiredActiveMode(
                      {scheduler::FrameRateMode{120_Hz, kMode120}, Event::None}));
    ASSERT_NE(std::nullopt, mDisplay->getDesiredActiveMode());
    EXPECT_FRAME_RATE_MODE(kMode120, 120_Hz, *mDisplay->getDesiredActiveMode()->modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getDesiredActiveMode()->event);

    EXPECT_FRAME_RATE_MODE(kMode90, 90_Hz, *mDisplay->getUpcomingActiveMode().modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getUpcomingActiveMode().event);

    EXPECT_EQ(OK,
              mDisplay->initiateModeChange(*mDisplay->getDesiredActiveMode(), constraints,
                                           &timeline));
    EXPECT_FRAME_RATE_MODE(kMode120, 120_Hz, *mDisplay->getUpcomingActiveMode().modeOpt);
    EXPECT_EQ(Event::None, mDisplay->getUpcomingActiveMode().event);

    mDisplay->clearDesiredActiveModeState();
    ASSERT_EQ(std::nullopt, mDisplay->getDesiredActiveMode());
=======
    EXPECT_TRUE(mDisplay->initiateModeChange(*mDisplay->getDesiredMode(), constraints, timeline));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode90, mDisplay->getPendingMode());

    EXPECT_EQ(Action::None, mDisplay->setDesiredMode(DisplayModeRequest(kDesiredMode120)));
    ASSERT_TRUE(mDisplay->getDesiredMode());
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode120, mDisplay->getDesiredMode());

    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode90, mDisplay->getPendingMode());

    EXPECT_TRUE(mDisplay->initiateModeChange(*mDisplay->getDesiredMode(), constraints, timeline));
    EXPECT_DISPLAY_MODE_REQUEST(kDesiredMode120, mDisplay->getPendingMode());

    mDisplay->clearDesiredMode();
    EXPECT_FALSE(mDisplay->getDesiredMode());
>>>>>>> android-14.0.0_r31
}

} // namespace
} // namespace android
