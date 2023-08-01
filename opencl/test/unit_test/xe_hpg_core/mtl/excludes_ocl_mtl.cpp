/*
 * Copyright (C) 2022-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/test/common/test_macros/hw_test_base.h"

HWTEST_EXCLUDE_PRODUCT(CommandStreamReceiverFlushTaskDg2AndLaterTests, givenProgramExtendedPipeControlPriorToNonPipelinedStateCommandEnabledWhenPerDssBackedBufferThenThereIsPipeControlPriorToIt_MatcherIsRTCapable, IGFX_METEORLAKE);
HWTEST_EXCLUDE_PRODUCT(CommandStreamReceiverFlushTaskXeHPAndLaterTests, givenProgramExtendedPipeControlPriorToNonPipelinedStateCommandEnabledAndStateSipWhenItIsRequiredThenThereIsPipeControlPriorToIt, IGFX_METEORLAKE);
HWTEST_EXCLUDE_PRODUCT(CommandStreamReceiverFlushTaskXeHPAndLaterTests, givenProgramExtendedPipeControlPriorToNonPipelinedStateCommandEnabledAndStateBaseAddressWhenItIsRequiredThenThereIsPipeControlPriorToIt, IGFX_METEORLAKE);
HWTEST_EXCLUDE_PRODUCT(AggregatedSmallBuffersDefaultTest, givenDifferentFlagValuesAndSingleOrMultiDeviceContextWhenCheckIfEnabledThenReturnCorrectValue, IGFX_METEORLAKE);
