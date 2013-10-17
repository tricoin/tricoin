// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (1,   uint256("0x8fe2901fc0999bc86ea2668c58802ee87165438166d18154f1bd4f917bf25e0f"))
        (2,    uint256("0x08019f74ff736f7723c14ce5b2eef9623eb1d86c6bc3262bbd0b56788e6ccff2"))
        (7,   uint256("0x4530df06d98fc77d04dab427630fc63b45f10d2b0ad3ad3a651883938986d629"))
        (10000,  uint256("0x3c29f365a04d8bf17b68d604c4cb6ed3e14c1b42a16a8ab923d9a978eff18e16"))
        (15420,  uint256("0xfded6a374d071f59d738a3009fc4d8461609052c3e7e91aa89146550d179c1b0"))
        (16000,  uint256("0x683517a8cae8530f39e636f010ecd1750665c3d91f57ba71d6556535972ab328"))
        (25000,  uint256("0x994102823b2e2434281878c774037c94502a345543e716d81380a8b5568620f3"))
        (40000,  uint256("0xcc045b52fe3519281627961949001040110a0e81429170e3f071450529b03eba"))
        (50000,  uint256("0x9d67c13afc8c46305395cfe08add094040e8e298349365b40b68c22bf49419a0"))
        (60000,  uint256("0x513056fb0206439021a58ce9d007a63fdca6c256cab340966359bef094d826d9"))
        (70000, uint256("0x262e04e7185b07522704ac712dc01adb70e3d287ba2eb1b657a16e9118f5f93b"))
        (77777, uint256("0xf5c98062cb1ad75c792a1851a388447f0edd7cb2271b67ef1241a03c673b7735"))
        (77778, uint256("0xd13f93f9fdac82ea26ed8f90474ed2449c8c24be50a416e43c323a38573c30e5"))
        (80000, uint256("0x43694915fa334a5adb70130ec90440a71de93a18ed2d2d00e4182b700e67bf9c"))
        (86075, uint256("0xf729b4087771932bb01388b0005c7e6cb0a7b85b09ed5eff3912229cd372c4e1")) // Last block before official build.
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1373072747, // * UNIX timestamp of last checkpoint block
        1664367,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        10000.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        (   546, uint256("000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70"))
        ( 35000, uint256("2af959ab4f12111ce947479bfcef16702485f04afd95210aa90fde7d1e4a64ad"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1369685559,
        37581,
        300
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (fTestNet) return 0; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (fTestNet) return NULL; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
