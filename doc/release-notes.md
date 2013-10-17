0.8.5.2 changes
===============

Workaround negative version numbers serialization bug.

Fix out-of-bounds check (Tricoin currently does not use this codepath, but we apply this
patch just to match Bitcoin 0.8.5.)

CVE-2013-5700 Bloom: filter crash issue - Tricoin 0.8.3.7 disabled bloom by default so was 
unaffected by this issue, but we include their patches anyway just in case folks want to 
enable bloomfilter=1.

CVE-2013-4165: RPC password timing guess vulnerability

CVE-2013-4627: Better fix for the fill-memory-with-orphaned-tx attack

Fix multi-block reorg transaction resurrection.

Fix non-standard disconnected transactions causing mempool orphans.  This bug could cause 
nodes running with the -debug flag to crash, although it was lot less likely on Tricoin 
as we disabled IsDust() in 0.8.3.x.

Mac OSX: use 'FD_FULLSYNC' with LevelDB, which will (hopefully!) prevent the database 
corruption issues have experienced on OSX.

Add height parameter to getnetworkhashps.

Add money supply information to RPC

Minor efficiency improvement in block peer request handling.

0.8.3.7 changes
===============

Fix CVE-2013-4627 denial of service, a memory exhaustion attack that could crash low-memory nodes.

Fix a regression that caused excessive writing of the peers.dat file.

Backport IRC seed

I2P Library upgrade

Add coin control

Add option for bloom filtering.

Fix Hebrew translation.
