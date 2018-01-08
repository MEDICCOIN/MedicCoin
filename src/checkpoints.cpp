// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
		(0, uint256("0x0000ac3f71507372db48ac2626bed6c1321cade7292431b274647673ab60a7ee"))
		(1, uint256("0x974c66768a079b1ddc38613e6733cba24470c7b50c7ccae64240e42178aedf9d"))
		(5, uint256("0x9a64ec448b309b3fb8c7d41e25c6ddf3888f822a37bcc0d0bc0cf6875c1b02c0"))
		(16, uint256("0x8595319d2d62df522fd05bf79a8753adc475c8b150107d1d037252bbf44cc9d6"))
		(25, uint256("0xc1b9be6f2f88bb42e323bf0b482f6aadc52c527352931c64ab9344c4e84d1fda"))
		(29, uint256("0x35d5844e193539238bca7123e598c8a5130addc30588e082141a7f0ddd24537f"))
		(32, uint256("0x6e25363b25e1aa000f7801704eb550e35927b31029490f68932c35ec36eba99a"))
		(57, uint256("0x5f653de9838f5cb63a1b5cba15b1b0144a2d77c4eb6ea9e0dbe5806df0525a5d"))
		(67, uint256("0x8f223d8d2bff7ed8f03ecf3c721a7e2ae0a4d08d1c42e166c54c5c823453771d"))
		(92, uint256("0x753fd60e4accb85280ca5b053ce684ba35d8716ee3bc5503e4742bc4fd4d2e32"))
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
