/*
 * Project: curve
 * Created Date: Saturday December 29th 2018
 * Author: hzsunjianliang
 * Copyright (c) 2018 netease
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <glog/logging.h>
#include "src/mds/nameserver2/clean_core.h"
#include "test/mds/nameserver2/mock_namespace_storage.h"

using ::testing::_;
using ::testing::Return;

namespace curve {
namespace mds {

TEST(CleanCore, test) {
    auto storage = new MockNameServerStorage();
    auto cleanCore = new CleanCore(storage);

    {
        // delete ok (no, segment)
        EXPECT_CALL(*storage, GetSegment(_, _, _))
        .WillRepeatedly(Return(StoreStatus::KeyNotExist));


        EXPECT_CALL(*storage, DeleteSnapshotFile(_, _))
        .Times(1)
        .WillOnce(Return(StoreStatus::OK));

        FileInfo cleanFile;
        cleanFile.set_length(kMiniFileLength);
        cleanFile.set_segmentsize(DefaultSegmentSize);
        TaskProgress progress;
        ASSERT_EQ(cleanCore->CleanSnapShotFile(cleanFile, &progress),
            StatusCode::kOK);

        ASSERT_EQ(progress.GetStatus(), TaskStatus::SUCCESS);
        ASSERT_EQ(progress.GetProgress(), 100);
    }
    {
        // all ok , but do DeleteFile namespace meta error
        EXPECT_CALL(*storage, GetSegment(_, _, _))
        .WillRepeatedly(Return(StoreStatus::KeyNotExist));


        EXPECT_CALL(*storage, DeleteSnapshotFile(_, _))
        .WillOnce(Return(StoreStatus::InternalError));

        FileInfo cleanFile;
        cleanFile.set_length(kMiniFileLength);
        cleanFile.set_segmentsize(DefaultSegmentSize);
        TaskProgress progress;
        ASSERT_EQ(cleanCore->CleanSnapShotFile(cleanFile, &progress),
            StatusCode::kSnapshotFileDeleteError);
    }

    {
        // get segment error
        EXPECT_CALL(*storage, GetSegment(_, _, _))
        .Times(1)
        .WillOnce(Return(StoreStatus::InternalError));

        FileInfo cleanFile;
        cleanFile.set_length(kMiniFileLength);
        cleanFile.set_segmentsize(DefaultSegmentSize);
        TaskProgress progress;
        ASSERT_EQ(cleanCore->CleanSnapShotFile(cleanFile, &progress),
            StatusCode::kSnapshotFileDeleteError);
    }
    {
        // get segment ok, DeleteSnapShotChunk Error
    }
    {
        // get segment ok, DeleteSnapShotChunk ok, DeleteSegment error
    }
    delete storage;
}


}  // namespace mds
}  // namespace curve