#include "pch.h"


#include "Handles.h"

#include "ACU/TypeInfo.h"
#include "ACU/AtomGraph.h"
#include "ACU/AtomGraphNode.h"
#include "ACU/AtomNullStateNode.h"
#include "ACU/AtomGraphStateNode.h"
#include "ACU/AtomStateMachineNode.h"
#include "ACU/AtomLayeringStateNode.h"
#include "ACU/AtomCustomTransitionSystem.h"
#include "ACU/AtomTransitionCellDescription.h"

#include "ACU/AtomAnimationDataNode.h"
#include "ACU/AtomAnimationRootNode.h"

#include "ACU/AtomStateTransitionTarget.h"
#include "ACU/AtomCondition.h"
#include "ACU/AtomConditionExpression.h"

#include "ACU_SharedPtrs.h"
#include "ACU_DefineNativeFunction.h"
#include "Experimental_StrongPtr.h"

#include "AnimationTools/UsefulHandles.h"



static std::array<const char*, 541> graphVariables_NewDemo_DEV = {
    "bool AButton",  // 0xf378a91b/4084771099
    "int AIDescriptor_Gender",  // 0x2f6fc249/795853385
    "int AIDescriptor_WealthClass",  // 0x35f43876/905197686
    "bool ActingIntroTransition",  // 0x39801f0/60293616
    "int ActingIntroType",  // 0x5517738c/1427600268
    "bool ActingReInit",  // 0xcaa1daa1/3399604897
    "scalar AdjustedReloadTime",  // 0x66e8aadc/1726524124
    "scalar AimingArcAngle",  // 0xf9e14196/4192289174
    "scalar AimingBoundingAngle",  // 0x5c6b9901/1550555393
    "bool AimingLanternDLC",  // 0x84acc1ea/2225914346
    "vector3 AimingTarget",  // 0x542266f9/1411540729
    "bool AllowedToTurn",  // 0x660ed30a/1712247562
    "scalar Altitude",  // 0x42bcc764/1119668068
    "scalar AngleToSightRabbit",  // 0x6b47f5b3/1799878067
    "bool Anticipate",  // 0x7e8488e4/2122615012
    "int ArrestGuardIndex",  // 0x729422e3/1922310883
    "bool Arrested",  // 0x94fd7db2/2499640754
    "bool AssassinateFromCover",  // 0xae2ebf8a/2922299274
    "bool AssassinateFromHideSpot",  // 0x25be10e8/633213160
    "bool AssassinateFromSurrender",  // 0x1c8ae9a3/478865827
    "bool AssassinateOverCover",  // 0xd037e82b/3493324843
    "int AssassinateState",  // 0x4b8fdbc2/1267719106
    "int AssassinateType",  // 0x28035d6e/671309166
    "scalar AssassinatedCoverHeight",  // 0xa8bf5cf6/2831113462
    "bool AssassinatedFromHideSpot",  // 0xfb9f51a6/4221522342
    "bool Assassinated_Ground",  // 0xe280942c/3800077356
    "bool AssassinationLoopSync",  // 0xeec37991/4005788049
    "bool AtEdge",  // 0x8de49d21/2380569889
    "int AttackFrom",  // 0x6288a4f0/1653122288
    "int AttackState",  // 0x4ef0c3de/1324401630
    "scalar Auto",  // 0xc6888ac4/3330837188
    "bool AutoSyncEntities",  // 0x2bc4cfde/734318558
    "bool BButton",  // 0xc290b386/3264263046
    "scalar BankingAngle",  // 0x2171a398/561095576
    "bool BankingImpulsion",  // 0x30c75ff4/818372596
    "scalar BankingRatio",  // 0xdd905eee/3717226222
    "bool Beam",  // 0x11a00162/295698786
    "int BeamDirection",  // 0x8da5f8fc/2376464636
    "scalar BeamLength",  // 0x2e02cc27/771935271
    "bool BeamStep",  // 0xb7d198a6/3083966630
    "bool BeamThrow",  // 0x5fbd530b/1606243083
    "bool BeamTrunkAround",  // 0xd3bf559d/3552531869
    "bool Block",  // 0x42dab826/1121630246
    "int BlockStates",  // 0xfddbd7d6/4259043286
    "scalar BodyBankingAngle",  // 0xa1670d1b/2707885339
    "bool Braking",  // 0x2fc97b66/801733478
    "int CWL_Character_Gender",  // 0x333c10c1/859574465
    "int CWL_Character_Mood",  // 0xa6f0f45f/2800809055
    "int CWL_Character_Type",  // 0x29170d80/689376640
    "bool CWL_EnableReactionSystem",  // 0xdfdcace8/3755781352
    "scalar CWL_LookAtTargetAngle",  // 0x6486c6fd/1686554365
    "scalar CWL_LookAtTargetAngle_H",  // 0xd7e80635/3622307381
    "scalar CWL_LookAtTargetAngle_V",  // 0x2de73b56/770128726
    "scalar CWL_LookAtTargetDistance",  // 0x20951fc2/546643906
    "vector3 CWL_LookAtTargetPosition",  // 0x7a2b61b6/2049663414
    "int CWL_LookAtTargetType",  // 0xe0f56330/3774178096
    "animationref CWL_Nav_Anim0",  // 0xa40a5d2a/2752142634
    "animationref CWL_Nav_Anim1",  // 0xd30d6dbc/3540872636
    "animationref CWL_Nav_Anim10",  // 0x36df2deb/920595947
    "animationref CWL_Nav_Anim11",  // 0x41d81d7d/1104682365
    "animationref CWL_Nav_Anim12",  // 0xd8d14cc7/3637595335
    "animationref CWL_Nav_Anim13",  // 0xafd67c51/2950069329
    "animationref CWL_Nav_Anim14",  // 0x31b2e9f2/833808882
    "animationref CWL_Nav_Anim15",  // 0x46b5d964/1186322788
    "animationref CWL_Nav_Anim16",  // 0xdfbc88de/3753674974
    "animationref CWL_Nav_Anim17",  // 0xa8bbb848/2830874696
    "animationref CWL_Nav_Anim18",  // 0x3804a5d9/939828697
    "animationref CWL_Nav_Anim19",  // 0x4f03954f/1325634895
    "animationref CWL_Nav_Anim2",  // 0x4a043c06/1241791494
    "animationref CWL_Nav_Anim3",  // 0x3d030c90/1023610000
    "animationref CWL_Nav_Anim4",  // 0xa3679933/2741475635
    "animationref CWL_Nav_Anim5",  // 0xd460a9a5/3563104677
    "animationref CWL_Nav_Anim6",  // 0x4d69f81f/1298790431
    "animationref CWL_Nav_Anim7",  // 0x3a6ec889/980338825
    "animationref CWL_Nav_Anim8",  // 0xaad1d518/2865878296
    "animationref CWL_Nav_Anim9",  // 0xddd6e58e/3721848206
    "scalar CWL_Nav_MoveFacingAngle",  // 0x1caf9f3d/481271613
    "scalar CWL_Nav_MoveHeadingAngle",  // 0xb65a0734/3059353396
    "scalar CWL_Nav_MoveSpeed",  // 0x5f44a177/1598333303
    "int CWL_Nav_Type",  // 0x3f860402/1065747458
    "scalar CWL_Reaction_Count",  // 0x362e7d8/56813528
    "scalar CWL_Reaction_Intensity",  // 0x43a9a433/1135191091
    "scalar CWL_Reaction_Probability",  // 0x7db315b7/2108888503
    "scalar CWL_Reaction_TimeSinceLast",  // 0x587f1aca/1484724938
    "int CWL_Reaction_Type",  // 0xe4af39f9/3836688889
    "bool CanBeRevived",  // 0xd4d5beda/3570777818
    "scalar CharacterTurnAngle",  // 0x48ebbdd9/1223409113
    "int Character_Mood",  // 0x79936d88/2039704968
    "bool Child",  // 0xe3727b2d/3815930669
    "animationref CinematicAnimation",  // 0x7ad472e5/2060743397
    "scalar CinematicAnimationTime",  // 0x2cf6a276/754360950
    "animationref CinematicFacialAnimation",  // 0x8f94dc5d/2408897629
    "scalar CinematicFacialAnimationTime",  // 0x1a78fdff/444136959
    "vector3 Climb_IK_Left_Foot_POS",  // 0x1c7ed17a/478073210
    "quat Climb_IK_Left_Foot_ROT",  // 0x819e90b7/2174652599
    "vector3 Climb_IK_Left_Hand_POS",  // 0xf277fc32/4067949618
    "quat Climb_IK_Left_Hand_ROT",  // 0x6f97bdff/1872215551
    "vector3 Climb_IK_Right_Foot_POS",  // 0xbffff33f/3221222207
    "quat Climb_IK_Right_Foot_ROT",  // 0x221fb2f2/572502770
    "vector3 Climb_IK_Right_Hand_POS",  // 0x51f6de77/1375133303
    "quat Climb_IK_Right_Hand_ROT",  // 0xcc169fba/3424034746
    "int Collide",  // 0x47501439/1196430393
    "scalar ComboCount",  // 0x398253f2/964842482
    "int ConcurrentAction",  // 0xdbcf0134/3687776564
    "int ConcurrentActionSubStep",  // 0xf8e30963/4175628643
    "int CoordinatorNPCCount",  // 0xc11ed319/3240022809
    "scalar CornerAssistReaction",  // 0x855ffaa2/2237659810
    "int CosmeticOnly_Random_0_1000",  // 0xe86c15e5/3899397605
    "scalar CosmeticOnly_Scalar_Random_0_1000",  // 0xb4331dc8/3023248840
    "bool Coughing",  // 0x5e0f8cd5/1578077397
    "scalar CoverAngleVariation",  // 0x14aaa6c3/346728131
    "bool CoverBackHasCorner",  // 0xe9f409cd/3925084621
    "scalar CoverCornerDistance",  // 0xbc75f1cc/3161846220
    "scalar CoverDistanceBack",  // 0xdae72894/3672582292
    "scalar CoverDistanceFront",  // 0xb193b94d/2979248461
    "scalar CoverDistanceToAss",  // 0x5a1b60b8/1511743672
    "bool CoverEnemyClose",  // 0x7c256ab3/2082826931
    "bool CoverFacingRight",  // 0xe3781eb2/3816300210
    "bool CoverFrontHasCorner",  // 0xdc6ec131/3698245937
    "scalar CoverLength",  // 0xd3977e5a/3549920858
    "scalar CoverMinWidth",  // 0xbf48d95f/3209222495
    "scalar CoverMove",  // 0x32061676/839259766
    "bool CoverPassAround",  // 0x1029f1c5/271184325
    "scalar CoverPassAroundHeight",  // 0x78c5517/126637335
    "bool CoverPeekHeight",  // 0x27cbcfb6/667668406
    "bool CoverPeeking",  // 0x5565db54/1432738644
    "bool CoverRotateAroundCorner",  // 0xe748c849/3880306761
    "bool CoverRotateAroundCornerInside",  // 0xbe6db9cb/3194862027
    "bool CoverStopOnSpot",  // 0x2035cc25/540396581
    "bool CoverTargetCoverHeight",  // 0x37341889/926161033
    "bool CoverTargetFacingRight",  // 0xacd7b3c6/2899817414
    "bool CoverToCoverAction",  // 0x196c23e1/426517473
    "scalar CoverToCoverParallelDistance",  // 0x9571abf8/2507254776
    "scalar CoverToCoverPerpendicularDistance",  // 0x2040af45/541110085
    "bool Cover_Aim",  // 0x39446424/960783396
    "scalar Cover_Distance",  // 0xf0854654/4035266132
    "bool Cover_Gun",  // 0x42b71471/1119294577
    "bool Cover_Mid",  // 0x498225e4/1233266148
    "bool Cover_Vertical",  // 0x4461a849/1147250761
    "scalar Coverheight",  // 0x200b00d1/537592017
    "bool Cower",  // 0x4d0bc3f6/1292616694
    "int CrowdAction",  // 0x7d850daa/2105871786
    "scalar CrowdDensityFactor",  // 0x107a15ba/276436410
    "bool CrowdGentlePush",  // 0xd9742878/3648268408
    "int CrowdGentlePushType",  // 0x65b1be/6664638
    "scalar CrowdPushAngle",  // 0xcbbf7632/3418322482
    "scalar CrowdPushDistance",  // 0x1a6e8a0/27715744
    "scalar CrowdPushIntensity",  // 0xa5c69cbe/2781256894
    "bool CrowdShove",  // 0x7d248335/2099544885
    "int CrowdShoveType",  // 0xd736b97a/3610687866
    "int Crowd_Interaction",  // 0x5cd27065/1557295205
    "int CurrentDangerSourceLocation",  // 0xb46eceb4/3027160756
    "animationref CustomActionAnimation",  // 0x1f25406a/522535018
    "animationref CustomActionAnimation2",  // 0xb7ad130b/3081573131
    "animationref CustomActionAnimation3",  // 0xc0aa239d/3232375709
    "animationref CustomActionAnimation4",  // 0x5eceb63e/1590605374
    "animationref CustomActionAnimation5",  // 0x29c986a8/701073064
    "animationref CustomActionAnimation6",  // 0xb0c0d712/2965427986
    "scalar CustomActionBlendDuration",  // 0x696ad973/1768610163
    "scalar CustomActionBlendMaxAngle",  // 0xc2b665d/204170845
    "int CustomActionBlendType",  // 0xbc53720/197474080
    "scalar CustomActionDiffAngleToFacing",  // 0x3e8d42a0/1049445024
    "scalar CustomActionEntryRatio",  // 0xe636fe0e/3862363662
    "animationref CustomActionIntroAnimation",  // 0x3b00d17d/989909373
    "animationref CustomActionIntroAnimation2",  // 0x345ab35d/878359389
    "animationref CustomActionIntroAnimation3",  // 0x435d83cb/1130202059
    "animationref CustomActionIntroAnimation4",  // 0xdd391668/3711506024
    "animationref CustomActionIntroAnimation5",  // 0xaa3e26fe/2856199934
    "animationref CustomActionIntroAnimation6",  // 0x33377744/859273028
    "scalar CustomActionIntroBlendMaxAngle",  // 0xe9c4ef8a/3921997706
    "int CustomActionIntroBlendType",  // 0x2fe0a637/803251767
    "bool CustomActionIntroUseDiffAngleCache",  // 0xd428f3c8/3559453640
    "animationref CustomActionOutroAnimation",  // 0x900c83b2/2416739250
    "animationref CustomActionOutroAnimation2",  // 0x3f2a602e/1059741742
    "animationref CustomActionOutroAnimation3",  // 0x482d50b8/1210929336
    "animationref CustomActionOutroAnimation4",  // 0xd649c51b/3595158811
    "animationref CustomActionOutroAnimation5",  // 0xa14ef58d/2706306445
    "animationref CustomActionOutroAnimation6",  // 0x3847a437/944219191
    "scalar CustomActionOutroBlendMaxAngle",  // 0x4563de68/1164172904
    "int CustomActionOutroBlendType",  // 0x84ecf4f8/2230121720
    "bool CustomActionOutroUseDiffAngleCache",  // 0xb3d13031/3016831025
    "bool CustomActionReEntry",  // 0xd2f862db/3539493595
    "int CustomActionStep",  // 0xccde78ae/3437131950
    "bool CustomActionUseDiffAngleCache",  // 0x88c9ab33/2294917939
    "int CustomAction_ExitType",  // 0xcb9fb14f/3416240463
    "scalar DONT_USE_TestNBB",  // 0x33a5cb44/866503492
    "bool Dead",  // 0x4d17e61a/1293411866
    "int DeathReason",  // 0x6c553023/1817522211
    "int DeathState",  // 0x269ed88f/647944335
    "int DeathType",  // 0xe4e4fb7b/3840211835
    "bool DebugReplica",  // 0x8b09dce9/2332679401
    "scalar DenseCrowdDistanceY",  // 0x44c26813/1153591315
    "bool Die",  // 0x135b937/20298039
    "scalar DiffAngleToCoverFacing",  // 0xb13bf8ba/2973497530
    "scalar DiffAngleToFacing",  // 0x85045deb/2231655915
    "scalar DiffAngleToFacingAssassination",  // 0xb668000a/3060269066
    "scalar DiffAngleToTarget",  // 0x4e6b7b9e/1315666846
    "scalar DiffVerticalAngleToTarget",  // 0x7da8e4bb/2108220603
    "int Direction",  // 0xbcbb5310/3166393104
    "bool DisableWeaponOut",  // 0xd81c558a/3625735562
    "scalar DistToStopDst",  // 0xaf961eaa/2945851050
    "scalar DistanceToAimTarget",  // 0x16348098/372539544
    "bool DiveOnSpot",  // 0xd7e278c2/3621943490
    "bool DoBeamShuffle",  // 0xf0b5bec2/4038442690
    "bool DoWhistle",  // 0x24a9c502/615105794
    "scalar DodgeAngle",  // 0xb2ea5e0a/3001703946
    "int DodgeFrom",  // 0x6f0a8ac9/1862961865
    "scalar DodgeLength",  // 0x98c6a413/2563154963
    "bool DropIn",  // 0x1a1c6d60/438070624
    "bool Eden_Apple_DLC",  // 0xd0766058/3497418840
    "bool Eden_Apple_DLC_Charging",  // 0x59ab05f9/1504380409
    "scalar EdgeDistanceX",  // 0x834afea8/2202730152
    "scalar EdgeDistanceY",  // 0xf44dce3e/4098739774
    "int ElevatorState",  // 0xa43c1abf/2755402431
    "bool EnableLookAt",  // 0x39f88915/972589333
    "bool EnableWeaponContact",  // 0xe96851b9/3915927993
    "scalar EntryRatio",  // 0x8bfb9ca3/2348522659
    "bool ExitState",  // 0x275494c0/659854528
    "bool FastSheath",  // 0x6f415d56/1866554710
    "int FightAction",  // 0xaa01c6a6/2852243110
    "int FightCombo",  // 0xc4f524ca/3304400074
    "scalar FightContextualEntryAngle",  // 0xfa4f8af9/4199516921
    "scalar FightContextualObstacletDistance",  // 0x5f56d688/1599526536
    "int FightContextualType",  // 0x63ef942a/1676645418
    "bool FightEntered",  // 0x52cf2c4a/1389309002
    "bool FightExited",  // 0x8a179cd6/2316803286
    "bool FightLoopSync",  // 0x1f96383d/529938493
    "int FightNavigationState",  // 0xc467e17a/3295142266
    "bool FightPrototype",  // 0x6f4b0ec7/1867189959
    "int FightStagingEmotion",  // 0x5a5ca598/1516021144
    "int FightState",  // 0xd65ac67b/3596273275
    "int FightVersion",  // 0x9e45ea3c/2655382076
    "bool FirstTargetAvailable",  // 0x7707ba5b/1996995163
    "scalar FootIkMaxObstacleHeight",  // 0x1024ac58/270838872
    "bool ForceInterrupt",  // 0xe8cd3a59/3905763929
    "bool ForcedIdle",  // 0xce061b22/3456506658
    "int FreeFallState",  // 0xeab9db2b/3938048811
    "int GeneralState",  // 0xdf85463d/3750053437
    "bool GetAssassinatedFromCover",  // 0xf87d1fd6/4168949718
    "bool GetAssassinatedOverCover",  // 0x86644877/2254719095
    "bool GetHit",  // 0x8bceed32/2345594162
    "bool GetPoisoned",  // 0xc47dd0f6/3296579830
    "int GuardAction",  // 0xafa2b0a7/2946674855
    "scalar GuardActionAngle",  // 0xed564e39/3981856313
    "int GuardActionDetectiveAreaRole",  // 0xa9036015/2835570709
    "scalar GuardActionDistance",  // 0xc3bae140/3283804480
    "bool GuardActionEnteredConflict",  // 0xeeb85f25/4005060389
    "scalar GuardActionInitialPosAngle",  // 0x831ddd3e/2199772478
    "scalar GuardActionInitialPosDistance",  // 0x7d97b66a/2107094634
    "bool GuardActionIsStationary",  // 0x1d257d73/488996211
    "int GuardActionLastActive",  // 0xb1f7b220/2985800224
    "int GuardActionSelfFaction",  // 0x2f9c08bb/798755003
    "int GuardActionStimulus",  // 0x48c3e452/1220797522
    "int GuardActionSubStep",  // 0x72aa2125/1923752229
    "int GuardActionTargetFaction",  // 0xe4359a41/3828718145
    "scalar GuardActionTargetOrientation",  // 0xa00721a4/2684821924
    "bool GuardActionTargetWeaponOut",  // 0x80cd7fa7/2160951207
    "scalar GuardActionVerticalAngle",  // 0x5ffc9daa/1610390954
    "bool GuardSupporter",  // 0xf8a97c66/4171856998
    "scalar HackingVariationsIndex",  // 0x1b0e3674/453916276
    "bool HasDLCCloak",  // 0xdc1976c9/3692656329
    "bool HasWideHips",  // 0x8ce8468d/2364032653
    "bool HayStack",  // 0xd8fdbe1a/3640507930
    "int HaystackDiveDirection",  // 0xfc00ecfc/4227919100
    "scalar Height",  // 0xf2e1e039/4074889273
    "scalar HeightDifferential",  // 0x5842c4cc/1480770764
    "bool HighClass",  // 0xcf930fb/217657595
    "bool HighProfile",  // 0xc2747d72/3262414194
    "bool HighProfileAssassination",  // 0xbfe51cfd/3219463421
    "scalar HitAngle",  // 0x16a5ddc0/379968960
    "scalar HitDealerDistance",  // 0xd10ed1a7/3507409319
    "scalar HitDirection",  // 0xc333cd80/3274952064
    "int HitLocation",  // 0x457bc0d1/1165738193
    "scalar HitTimeRemaining",  // 0x6b0cb2c3/1795994307
    "scalar Horizontal",  // 0x1c5e079e/475924382
    "bool Hungover",  // 0xad5e408f/2908635279
    "bool Hurt",  // 0x67bfab24/1740614436
    "int HurtType",  // 0x661a3473/1712993395
    "vector3 IK_Hips_Pos",  // 0x888ec56c/2291058028
    "scalar IK_Hips_Ratio",  // 0x95fd1614/2516391444
    "quat IK_Hips_Rot",  // 0x156e84a1/359564449
    "vector3 IK_LF_Pos",  // 0x4ec0e743/1321264963
    "scalar IK_LF_Ratio",  // 0x5281a9d3/1384229331
    "quat IK_LF_Rot",  // 0xd320a68e/3542132366
    "vector3 IK_LH_Pos",  // 0xf1f05922/4059060514
    "scalar IK_LH_Ratio",  // 0x2bed28e/46060174
    "quat IK_LH_Rot",  // 0x6c1018ef/1812994287
    "vector3 IK_RF_Pos",  // 0x771c84a8/1998357672
    "scalar IK_RF_Ratio",  // 0x3bb08aca/1001425610
    "quat IK_RF_Rot",  // 0xeafcc565/3942434149
    "vector3 IK_RH_Pos",  // 0xc82c3ac9/3358341833
    "scalar IK_RH_Ratio",  // 0x6b8ff197/1804595607
    "quat IK_RH_Rot",  // 0x55cc7b04/1439464196
    "scalar IK_Ratio",  // 0x13a7fd68/329776488
    "vector3 IK_Shoulders_Pos",  // 0x2827933e/673682238
    "scalar IK_Shoulders_Ratio",  // 0x9f787daa/2675473834
    "quat IK_Shoulders_Rot",  // 0xb5c7d2f3/3049771763
    "bool IfDebugFeature",  // 0x7aaaaaf1/2058005233
    "bool InCover",  // 0xf4eadf22/4109033250
    "bool InDenseCrowd",  // 0xa28fff34/2727345972
    "bool InHideSpot",  // 0xb217164/186741092
    "bool InboxNewItem",  // 0x64521f03/1683103491
    "scalar IncomingCoverAngleVariation",  // 0xf0d9cc34/4040805428
    "scalar IncomingSlopeAngle",  // 0x492a8a04/1227524612
    "bool Indoor",  // 0x1436b81a/339130394
    "bool InteractionAccepted",  // 0xbde13410/3185652752
    "bool Interrupt",  // 0x790ec47a/2031010938
    "int Interruptibility",  // 0x6d4622eb/1833313003
    "int IntimidateState",  // 0xacb811ec/2897744364
    "bool Investigate",  // 0x52606110/1382048016
    "bool IsActing",  // 0x1f156dfb/521498107
    "bool IsAssassinationInAwarenessZone",  // 0xb6309910/3056638224
    "bool IsAsssassinateAttacker",  // 0x24766160/611737952
    "bool IsBehindDoor",  // 0x759b451a/1973110042
    "bool IsBerserk",  // 0x4ea7db8a/1319623562
    "bool IsBerserkInitial",  // 0x9cc911eb/2630423019
    "bool IsBulkStation",  // 0x2c2b039/46313529
    "bool IsClimbingLadder",  // 0xa0c4046d/2697200749
    "bool IsCrowdNPC",  // 0xbd90b3b0/3180377008
    "bool IsCrowdPushed",  // 0xd1bfd285/3519009413
    "bool IsDrunk",  // 0xde285267/3727184487
    "bool IsFacialActing",  // 0x7c4c950e/2085393678
    "bool IsFreeAiming",  // 0x4b6f8f83/1265602435
    "bool IsGuardPost",  // 0x59da7f9e/1507491742
    "bool IsInStrafe",  // 0xa544f9d2/2772761042
    "bool IsLikelyToKill",  // 0xecffa4a6/3976176806
    "bool IsNPC",  // 0xc5212fe3/3307286499
    "bool IsOwnBomb",  // 0x35f8cf71/905498481
    "bool IsPassenger",  // 0x84ebd6cd/2230048461
    "bool IsPlayingSpecificAnimation",  // 0xf9324be6/4180823014
    "bool IsRaining",  // 0xe170c491/3782263953
    "bool IsSearchInterior",  // 0x7e9f7561/2124379489
    "bool IsSniper",  // 0x96362047/2520129607
    "bool IsTwoHandedSword",  // 0xee3a9c0c/3996818444
    "bool IsWindow",  // 0xb3fa9151/3019542865
    "bool JoinInProgress",  // 0x94b48944/2494859588
    "bool Juke",  // 0x3c06ea45/1007086149
    "int Kicker",  // 0x8befbdee/2347744750
    "int LOD_CurrentLODLevel",  // 0x720604e7/1912997095
    "scalar LOD_DistanceFromCamera",  // 0x38e856ef/954750703
    "int LadderState",  // 0xe7caee09/3888836105
    "scalar LandingHeight",  // 0x7989e106/2039079174
    "int LandingState",  // 0xb4eb28a5/3035310245
    "bool LanternDLC_Collision",  // 0x6676d8a0/1719064736
    "bool LatoucheWalk",  // 0x44e2a880/1155704960
    "animationref LayeredCustomActionAnimation",  // 0x32e68c2d/853969965
    "animationref LayeredCustomActionIntroAnimation",  // 0x5e8b766c/1586198124
    "animationref LayeredCustomActionOutroAnimation",  // 0xf58724a3/4119274659
    "bool LayeredCustomActionReEntry",  // 0x902f36e5/2419013349
    "int LayeredCustomActionSteps",  // 0x562770c4/1445425348
    "scalar LeftStickAngle",  // 0xcf78be3c/3480796732
    "scalar LeftStickH",  // 0xba29cdf/195206367
    "scalar LeftStickLastAngle",  // 0xba8fcf3d/3129986877
    "scalar LeftStickLength",  // 0x5701a36a/1459725162
    "scalar LeftStickV",  // 0xf1ada1bc/4054688188
    "bool LightAttack",  // 0x48a23f8b/1218592651
    "int LivingBreathingReaction",  // 0x6a9b3030/1788555312
    "int LockPickResult",  // 0x8448d482/2219365506
    "vector3 LookAtTarget",  // 0x7e5ba346/2119934790
    "scalar LookAtTargetAngle",  // 0x9878e426/2558059558
    "scalar LookAtTargetAngle_H",  // 0x6044a7fb/1615112187
    "scalar LookAtTargetAngle_V",  // 0x9a4b9a98/2588646040
    "bool LurkingIdle",  // 0xaef12ad0/2935040720
    "scalar MotionSpeed",  // 0x7399e779/1939466105
    "scalar MovementSpeed",  // 0xf8928506/4170351878
    "int MovementType",  // 0x5af2fabd/1525873341
    "int NPCCoordinatorIndex",  // 0x980bf9e5/2550921701
    "int NPC_Nav_Type",  // 0x79194be2/2031700962
    "bool NPC_ObjectCarrier",  // 0x34f048a0/888162464
    "int NPC_ObjectCarrierNavType",  // 0xa2175b05/2719439621
    "bool NextCoverCornerFacingRight",  // 0x754d18ec/1967986924
    "bool Obstructed",  // 0x9bb09e85/2612043397
    "int Parkour",  // 0xbdec3825/3186374693
    "int ParkourElementIncoming",  // 0x4d509a44/1297128004
    "int ParkourElementOutgoing",  // 0x6a4cea9e/1783425694
    "int ParkourMode",  // 0xcd777060/3447156832
    "int ParryState",  // 0xc597add/207190749
    "bool PeekingBack",  // 0x54f8a6ae/1425581742
    "bool PermanentIdleActivateMood",  // 0xf0ae1966/4037941606
    "scalar PermanentIdleIntensity",  // 0xa8f9d9cd/2834946509
    "int PermanentIdleType",  // 0x6e9e4860/1855866976
    "bool Pilotis",  // 0x800e25a6/2148410790
    "bool PlayCustomAction",  // 0x652bf7c3/1697380291
    "bool PlayLayeredCustomAction",  // 0xfbbbc0f1/4223385841
    "scalar PlayerSpeed",  // 0xb7447e0d/3074719245
    "int PreparePrimaryWeaponSelf",  // 0x6e495572/1850299762
    "int PrepareSecondaryWeaponSelf",  // 0xfc2a930d/4230648589
    "int PreviousFightAction",  // 0x98ce0273/2563637875
    "int PrimaryEquippedWeaponSelf",  // 0xb3da2ef2/3017420530
    "int PrimaryWeaponAtk",  // 0x76a20ccb/1990331595
    "bool PrimaryWeaponDLC",  // 0x5243da21/1380178465
    "int PrimaryWeaponSelf",  // 0xd2a65240/3534115392
    "int PrimaryWeaponVic",  // 0x9e7c5d10/2658950416
    "bool Prisoner",  // 0x5df7a7ad/1576511405
    "bool ProudWalk",  // 0x3c6ab8b1/1013627057
    "int PushState",  // 0xedbc874f/3988555599
    "bool PushTargetIsSprinting",  // 0x422edce8/1110367464
    "int PushType",  // 0xa708b7cb/2802366411
    "int PushVariation",  // 0x19845286/428102278
    "scalar PushedAngle",  // 0x84f0d514/2230375700
    "scalar PushedDistance",  // 0x5e2f6196/1580163478
    "int PusherFaction",  // 0x36b3c815/917751829
    "int PusherPrimaryWeapon",  // 0x63af7abb/1672444603
    "scalar PusherToVictimAngle",  // 0x60cadcb7/1623907511
    "bool RTrigger",  // 0x3a9bc26d/983286381
    "quat ReachOrientation",  // 0x85d5646e/2245354606
    "vector3 ReachPosition",  // 0x401a535d/1075467101
    "int ReactionState",  // 0xf44258a6/4097988774
    "bool ReflexShot",  // 0xa9e7b83b/2850535483
    "bool ResetState",  // 0x870d4aec/2265795308
    "scalar ReviveTimeScale",  // 0xcee6f0ff/3471241471
    "int ReviveType",  // 0xbd8f5a23/3180288547
    "bool RiftBreakCover",  // 0xe243add/237255389
    "scalar RiftHailDirection",  // 0xce70dab9/3463502521
    "bool RiftHailState",  // 0x28f92677/687416951
    "scalar RiftHorizontal",  // 0xe8f82130/3908575536
    "int RiftPreviousGeneralState",  // 0x93679e28/2473041448
    "int RiftState",  // 0xa9acfa24/2846685732
    "int RiftThreatImpact",  // 0xde7f919c/3732902300
    "scalar RiftVertical",  // 0xc598850c/3315107084
    "int RiftWeapon",  // 0x7f55578b/2136299403
    "bool RiftWeaponFiring",  // 0xf4350b9/256069817
    "bool RiftWeaponHit",  // 0x6104fa57/1627716183
    "bool RiftWeaponReload",  // 0xf82eec5e/4163824734
    "scalar RightStickAngle",  // 0x2fa5e15f/799400287
    "scalar RightStickLength",  // 0x835a4ed7/2203733719
    "bool Scolding",  // 0x7a5f78de/2053077214
    "bool SecondTargetAvailable",  // 0xb4da814/189638676
    "int SecondaryWeaponAtk",  // 0x86057e31/2248506929
    "int SecondaryWeaponSelf",  // 0x8f3eee30/2403266096
    "int SecondaryWeaponVic",  // 0x6edb2fea/1859858410
    "bool Shooting_Fire",  // 0xb9d23a2c/3117562412
    "scalar Shooting_HipFireRatio",  // 0xc498bc75/3298344053
    "scalar Shooting_RateOfFire",  // 0xec9b9f0/248101360
    "bool Shooting_Strafe",  // 0x7cf92800/2096703488
    "vector3 Shooting_TargetPosition",  // 0x396b2c3c/963324988
    "scalar ShoulderBankingAngle",  // 0x489227b9/1217537977
    "bool SideLeap",  // 0xd12f529f/3509539487
    "scalar SideStepLength",  // 0x357cc17b/897368443
    "scalar SlopeAngle",  // 0xb1064648/2969978440
    "bool SlopeScramble",  // 0x403dcebf/1077792447
    "bool SlopeSlide",  // 0xc368ed23/3278433571
    "bool Sneak",  // 0x8af229cb/2331126219
    "animationref SpecificAnimation",  // 0x38eba872/954968178
    "scalar Speed",  // 0xcee7d1f2/3471299058
    "scalar SpeedFactor",  // 0xd6c2c506/3603088646
    "scalar StealthTargetDistance",  // 0x7fa92958/2141792600
    "bool Strafe",  // 0xb1d60d03/2983595267
    "scalar StrafeAngle",  // 0x501e7b/5250683
    "bool StrafeAngleChangeTooBig",  // 0x94985766/2493011814
    "scalar StrafeAngleDiff",  // 0xf57f1c26/4118748198
    "int StrafeState",  // 0xa3428989/2739046793
    "bool Stun",  // 0x53467e30/1397128752
    "bool Subject",  // 0x347307e6/879953894
    "bool Surrender",  // 0x170f0f22/386862882
    "bool Surrender_Kill",  // 0x178326d5/394471125
    "int SwimmingStyle",  // 0x7cc8e771/2093541233
    "scalar SyncA_TargetAngle",  // 0x1f1ca3dc/521970652
    "scalar SyncA_TargetDistance",  // 0x114ad6d6/290117334
    "scalar SyncA_TargetHeight",  // 0x3eb5eb2/65756850
    "scalar SyncB_TargetAngle",  // 0xa2d6cf12/2731986706
    "scalar SyncB_TargetDistance",  // 0x38826224/948068900
    "scalar SyncB_TargetHeight",  // 0x7f8a7b69/2139781993
    "scalar SyncC_TargetAngle",  // 0x7f401697/2134906519
    "scalar SyncC_TargetDistance",  // 0x96eaf3b5/2531980213
    "scalar SyncC_TargetHeight",  // 0xe2859a1f/3800406559
    "scalar SyncD_TargetAngle",  // 0x23310cf/36901071
    "scalar SyncD_TargetDistance",  // 0x6b130bc0/1796410304
    "scalar SyncD_TargetHeight",  // 0x874830df/2269655263
    "scalar SyncE_TargetAngle",  // 0xdfa5c94a/3752184138
    "scalar SyncE_TargetDistance",  // 0xc57b9a51/3313211985
    "scalar SyncE_TargetHeight",  // 0x1a47d1a9/440914345
    "bool Tackle",  // 0x1b0e78ef/453933295
    "bool TackleLoopSync",  // 0x638e82bd/1670283965
    "int TackleState",  // 0xc5f5c312/3321217810
    "scalar TargetAngle",  // 0xd7e18370/3621880688
    "scalar TargetAngleAssassination",  // 0x1bc28306/465732358
    "bool TargetChanged",  // 0x2f094fa0/789139360
    "scalar TargetCoverHeight",  // 0x58a31ff4/1487085556
    "scalar TargetDistance",  // 0x97a0d819/2543900697
    "scalar TargetDistanceAction",  // 0x680b591f/1745574175
    "scalar TargetDistanceAssassination",  // 0x56316c3d/1446079549
    "scalar TargetDistanceReal",  // 0x5859a0f8/1482268920
    "scalar TargetDistanceX",  // 0xd34e3e53/3545120339
    "scalar TargetDistanceXAssassination",  // 0x945912c3/2488865475
    "scalar TargetDistanceXSecond",  // 0xd1bccc86/3518811270
    "scalar TargetDistanceY",  // 0xa4490ec5/2756251333
    "scalar TargetDistanceYAssassination",  // 0x956f3b5/156693429
    "scalar TargetHeight",  // 0xdc434c51/3695397969
    "scalar TargetHeightAssassination",  // 0x6ebae69/116108905
    "scalar TargetSpeed",  // 0xd846388f/3628480655
    "scalar TargetWallAngle",  // 0xa4974703/2761377539
    "scalar TargetWallDistanceX",  // 0x8c0bf14b/2349592907
    "scalar TargetWallDistanceY",  // 0xfb0cc1dd/4211917277
    "scalar TargetWidth",  // 0x5b7a8356/1534755670
    "int TauntType",  // 0x58a25453/1487033427
    "scalar ThrowAimAngle",  // 0xb2685191/2993181073
    "scalar ThrowPitch",  // 0x111e9ca0/287218848
    "int ThrowState",  // 0x9512f082/2501046402
    "int TimeOfDay",  // 0x5209ddad/1376378285
    "scalar TimeRemaining",  // 0x1221b873/304199795
    "scalar TimeRemainingEntity0",  // 0x55a9e59d/1437197725
    "scalar TimeRemainingEntity1",  // 0x22aed50b/581883147
    "scalar TimeScale",  // 0x5feef115/1609494805
    "scalar TopLedgeDistance",  // 0xdec8d023/3737702435
    "bool Turn180",  // 0x5d1dbec7/1562230471
    "int TurnOnSpotType",  // 0xb69dae1d/3063787037
    "int Turns",  // 0x32571429/844567593
    "int UniqueNPCType",  // 0xc0de10e9/3235778793
    "bool UseAntiGravityRagdoll",  // 0x5027a990/1344776592
    "bool UseBipedFootIK",  // 0x43301f6d/1127227245
    "bool UseWeaponActionUpperbody",  // 0x794990ea/2034864362
    "scalar VShape_Depth",  // 0xb5a1df45/3047284549
    "scalar Variation",  // 0xe06ab149/3765088585
    "scalar Vertical",  // 0x510016ca/1358960330
    "scalar VirtualFootIkSlope",  // 0xf253554d/4065547597
    "bool Vshape",  // 0x7007428c/1879523980
    "scalar Vshape_Base",  // 0x52d88312/1389921042
    "int Vshape_Hips",  // 0xec8fd45c/3968848988
    "scalar Vshape_Left",  // 0xe80b9a1b/3893074459
    "scalar Vshape_Right",  // 0x7d5439ef/2102671855
    "scalar Vshape_Tilt",  // 0x8e53f485/2387866757
    "int Vshape_Type",  // 0x1eb22a5a/514992730
    "bool Wall",  // 0xb3c740c8/3016179912
    "scalar WallDistanceY",  // 0x67810631/1736508977
    "bool WallEject",  // 0x4c773550/1282880848
    "int WeaponAction",  // 0xffd34ea5/4292038309
    "bool WeaponContactMaster",  // 0x86251836/2250577974
    "scalar WeaponContactMaxDistance",  // 0x9b5c970c/2606536460
    "scalar WeaponContactMinDistance",  // 0x5751ec03/1464986627
    "bool Wounded",  // 0x75fc0318/1979450136
    "bool arrest",  // 0x2d28303f/757608511
    "bool cover_wall_hop",  // 0x724d10bf/1917653183
    "bool debugCCOToTarget",  // 0x814f356b/2169451883
    "scalar debugEntitySightDelta",  // 0xb7ec4fd0/3085717456
    "scalar debugFacingAngleDelta",  // 0xa8838d72/2827193714
    "scalar debugHeadingAngleDelta",  // 0xe0501daa/3763346858
    "bool isUnlocked",  // 0x9a091bf4/2584288244
    "bool walling_vert",  // 0x3a67c2a8/979878568
    "entityref SyncParticipant1",  // 0x324db80/52747136
    "entityref SyncSubject",  // 0xe35d816d/3814556013
};

class AtomGraphDumper
{
public:
    std::string m_currentIndent;
    class Indent
    {
    public:
        Indent(AtomGraphDumper& parent) : parent(parent) { parent.m_currentIndent += "  "; }
        ~Indent() { parent.m_currentIndent = parent.m_currentIndent.substr(2); }
    private:
        AtomGraphDumper& parent;
    };
    Indent IndentScoped() { return Indent{ *this }; }

public:
    struct IDOffsetData
    {
        uint32 offset;
        bool m_doDereference;
    };
    std::vector<IDOffsetData> m_currentOffsetsStack;
    ImGuiTextBuffer FormatTheCurrentOffsetStackForCPP()
    {
        uint32 currentAccumulatedTopmostOffset = 0;
        ImGuiTextBuffer buf;
        buf.append("({");
        for (size_t i = 0; i < m_currentOffsetsStack.size(); i++)
        {
            const IDOffsetData& nextOffset = m_currentOffsetsStack[i];
            if (nextOffset.m_doDereference)
            {
                const char* fmt = i > 0 ? ",%u" : "%u";
                buf.appendf(fmt, currentAccumulatedTopmostOffset + m_currentOffsetsStack[i].offset);
                currentAccumulatedTopmostOffset = 0;
            }
            else
            {
                currentAccumulatedTopmostOffset += nextOffset.offset;
            }
        }
        buf.appendf("}, %u)", currentAccumulatedTopmostOffset);
        return buf;
    }
    ImGuiTextBuffer FormatTheCurrentOffsetStackForReclassAndCheatEngine()
    {
        ImGuiTextBuffer buf;
        const char* rootAddr = "[[[[[[[[[[[[14521AAD0]+40]]+10]+60]+218]+0]+c8]+710]+1c90]+d0]+670]";
        for (size_t i = 0; i < m_currentOffsetsStack.size(); i++)
        {
            if (m_currentOffsetsStack[i].m_doDereference)
                buf.append("[");
        }
        buf.append(rootAddr);
        for (size_t i = 0; i < m_currentOffsetsStack.size(); i++)
        {
            buf.appendf("+%x", m_currentOffsetsStack[i].offset);
            if (m_currentOffsetsStack[i].m_doDereference)
                buf.append("]");
        }
        return buf;
    }
    class IDOffset
    {
    public:
        IDOffset(AtomGraphDumper& parent, uint32 newTopmostOffset) : parent(parent) { parent.m_currentOffsetsStack.push_back(IDOffsetData{ newTopmostOffset, true }); }
        ~IDOffset() { parent.m_currentOffsetsStack.pop_back(); }
    private:
        AtomGraphDumper& parent;
    };
    class IDOffset_NoDereference
    {
    public:
        IDOffset_NoDereference(AtomGraphDumper& parent, uint32 addToTopmostOffset) : parent(parent) { parent.m_currentOffsetsStack.push_back(IDOffsetData{ addToTopmostOffset, false }); }
        ~IDOffset_NoDereference() { parent.m_currentOffsetsStack.pop_back(); }
    private:
        AtomGraphDumper& parent;
    };
    IDOffset IDOffsetScoped(uint32 newTopmostOffset) { return IDOffset{ *this, newTopmostOffset }; }
    IDOffset_NoDereference IDOffsetScoped_NoDereference(uint32 addToTopmostOffset) { return IDOffset_NoDereference{ *this, addToTopmostOffset }; }
#define IDOFFSETS_PUSH(newTopmostOffset) auto _offset = IDOffsetScoped(newTopmostOffset);
#define IDOFFSETS_PUSH_NODEREFERENCE(addToTopmostOffset) auto _offsetPlus = IDOffsetScoped_NoDereference(addToTopmostOffset);
public:

    void DumpOutputPorts(AtomGraphNode& graphNode)
    {
        for (AtomOutputPort& output : graphNode.OutputPorts)
        {
            ImGui::LogText(
                "%s[out]"
                , m_currentIndent.c_str()
            );
            if (output.graphNode == &graphNode)
            {
                ImGui::LogText("==\n");
                continue;
            }
            DumpGraphNodeShort(*output.graphNode);
            if (IsNotSeenBefore(*output.graphNode))
            {
                DumpGraphNode(*output.graphNode);
            }
        }
    }
    void DumpInputPorts(AtomGraphNode& graphNode)
    {
        IDOFFSETS_PUSH(offsetof(AtomGraphNode, InputPorts));
        for (uint16 i = 0; i < graphNode.InputPorts.size; i++)
        {
            AtomInputPort& inp = graphNode.InputPorts[i];
            ImGui::LogText(
                "%s[in] BindingType: %d\n"
                , m_currentIndent.c_str()
                , inp.BindingType
            );
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH_NODEREFERENCE(i * sizeof(AtomInputPort));
            if (inp.BindingType == 9)
            {
                IDOFFSETS_PUSH(offsetof(AtomInputPort, outputPortIfTypeEq9_graphVarIdxIfEq0));
                AtomGraphNode* inpNode = inp.outputPortIfTypeEq9_graphVarIdxIfEq0->graphNode;
                {
                    IDOFFSETS_PUSH(offsetof(AtomOutputPort, graphNode));
                    ImGui::LogText(m_currentIndent.c_str());
                    DumpGraphNodeShort(*inpNode);
                    if (IsNotSeenBefore(*inpNode))
                    {
                        DumpGraphNode(*inpNode);
                    }
                }
            }
            else if (inp.BindingType == 0)
            {
                ImGui::LogText(
                    "%s GraphVar: %d== %s\n"
                    , m_currentIndent.c_str()
                    , inp.outputPortIfTypeEq9_graphVarIdxIfEq0
                    , graphVariables_NewDemo_DEV[(uint32&)inp.outputPortIfTypeEq9_graphVarIdxIfEq0]
                );
            }
        }
    }
    void DumpGraphNodeShort(AtomGraphNode& graphNode)
    {
        ImGui::LogText(
            "%llX %s %s %s\n"
            , &graphNode
            , graphNode.Unk028_GetTI().typeName
            , FormatTheCurrentOffsetStackForCPP().c_str()
            , FormatTheCurrentOffsetStackForReclassAndCheatEngine().c_str()
        );
    }
    void DumpGraphNode(AtomGraphNode& graphNode)
    {
        auto ind = IndentScoped();
        DumpInputPorts(graphNode);
        DumpOutputPorts(graphNode);
        DumpGraphNodeCustom(graphNode);
    }
    template<typename AtomGraphNodeSubcls>
    void DumpGraphNodeDetails(AtomGraphNodeSubcls& graphNode);
    template<>
    void DumpGraphNodeDetails(AtomAnimationDataNode& animNode)
    {
        SharedPtrNew<Animation>* sharedAnim = animNode.shared_Animation;
        ImGui::LogText(
            "%s%llu => %s\n"
            , m_currentIndent.c_str()
            , sharedAnim->handle
            , ACU::Handles::HandleToText(sharedAnim->handle).c_str()
        );
    }
    void DumpGraphNodeCustom(AtomGraphNode& graphNode)
    {
        if (std::string_view(graphNode.Unk028_GetTI().typeName) == "AtomAnimationDataNode")
        {
            DumpGraphNodeDetails(static_cast<AtomAnimationDataNode&>(graphNode));
        }
    }
    void DumpLayeringInfo(AtomLayeringInfo& layerInfo)
    {
        ImGui::LogText(
            "%s LayerId: %u\n"
            , m_currentIndent.c_str()
            , layerInfo.LayerId
        );
        ImGui::LogText(
            "%s anim:\n"
            , m_currentIndent.c_str()
        );
        {
            auto ind = IndentScoped();
            SharedPtrNew<Animation>* sharedAnim = layerInfo.shared_Animation;
            ImGui::LogText(
                "%s%llu => %s\n"
                , m_currentIndent.c_str()
                , sharedAnim->handle
                , ACU::Handles::HandleToText(sharedAnim->handle).c_str()
            );
        }
        ImGui::LogText(
            "%s state node:\n"
            , m_currentIndent.c_str()
        );
        {
            auto ind = IndentScoped();
            IDOFFSETS_PUSH(offsetof(AtomLayeringInfo, stateNode38));
            DumpStateNode(*layerInfo.stateNode38);
        }
    }
    template<typename AtomStateNodeSubcls>
    void DumpStateNodeDetails(AtomStateNodeSubcls& stateNode);
    template<>
    void DumpStateNodeDetails<AtomGraphStateNode>(AtomGraphStateNode& graphStateNode)
    {
        ImGui::LogText(
            "%s Nodes: %d\n"
            , m_currentIndent.c_str()
            , graphStateNode.Nodes.size
        );
        auto _ind = IndentScoped();
        IDOFFSETS_PUSH(offsetof(AtomGraphStateNode, Nodes));
        for (uint16 i = 0; i < graphStateNode.Nodes.size; i++)
        {
            AtomGraphNode* graphNode = graphStateNode.Nodes[i];
            IDOFFSETS_PUSH(i * sizeof(AtomGraphNode*));
            ImGui::LogText(
                "%s"
                , m_currentIndent.c_str()
            );
            DumpGraphNodeShort(*graphNode);
            if (IsNotSeenBefore(*graphNode))
            {
                DumpGraphNode(*graphNode);
            }
        }
    }
    void DumpStateTransitionTarget(AtomStateTransitionTarget& trTarget)
    {
        ImGui::LogText(
            "%s TargetStateIndex: #%d.\n"
            , m_currentIndent.c_str()
            , trTarget.TargetStateIndex
        );
        ImGui::LogText(
            "%s TransitionTime: %f.\n"
            , m_currentIndent.c_str()
            , trTarget.TransitionTime
        );
        ImGui::LogText(
            "%s conditionExpression:\n"
            , m_currentIndent.c_str()
        );
        auto _ind = IndentScoped();
        DumpConditionExpression(*trTarget.conditionExpression);
    }
    void DumpStateMachineInitialState(AtomInitialState& initState)
    {
        ImGui::LogText(
            "%s TargetStateIndex: #%d.\n"
            , m_currentIndent.c_str()
            , initState.TargetStateIndex
        );
        ImGui::LogText(
            "%s conditionExpression:\n"
            , m_currentIndent.c_str()
        );
        auto _ind = IndentScoped();
        DumpConditionExpression(*initState.conditionExpression);
    }
    template<>
    void DumpStateNodeDetails<AtomStateMachineNode>(AtomStateMachineNode& stateMachineNode)
    {
        ImGui::LogText(
            "%s InitialStates: %d\n"
            , m_currentIndent.c_str()
            , stateMachineNode.InitialStates.size
        );
        {
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH(offsetof(AtomStateMachineNode, InitialStates));
            for (uint16 i = 0; i < stateMachineNode.InitialStates.size; i++)
            {
                IDOFFSETS_PUSH_NODEREFERENCE(i * sizeof(AtomInitialState));
                AtomInitialState& initState = stateMachineNode.InitialStates[i];
                ImGui::LogText(
                    "%s initial state #%d:\n"
                    , m_currentIndent.c_str()
                    , i
                );
                auto _ind = IndentScoped();
                DumpStateMachineInitialState(initState);
            }
        }
        ImGui::LogText(
            "%s transitionTargets_98: %d\n"
            , m_currentIndent.c_str()
            , stateMachineNode.transitionTargets_98.size
        );
        {
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH(offsetof(AtomStateMachineNode, transitionTargets_98));
            for (uint16 i = 0; i < stateMachineNode.transitionTargets_98.size; i++)
            {
                AtomStateTransitionTarget* trTarget = stateMachineNode.transitionTargets_98[i];
                ImGui::LogText(
                    "%s transition #%d.:\n"
                    , m_currentIndent.c_str()
                    , i
                );
                auto _ind = IndentScoped();
                IDOFFSETS_PUSH(i * sizeof(AtomStateTransitionTarget*));
                DumpStateTransitionTarget(*trTarget);
            }
        }
        ImGui::LogText(
            "%s States: %d\n"
            , m_currentIndent.c_str()
            , stateMachineNode.States.size
        );
        {
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH(offsetof(AtomStateMachineNode, States));
            for (uint16 i = 0; i < stateMachineNode.States.size; i++)
            {
                IDOFFSETS_PUSH(i * sizeof(AtomStateNode*));
                AtomStateNode* stateNode = stateMachineNode.States[i];
                ImGui::LogText(
                    "%s state #%d.:\n"
                    , m_currentIndent.c_str()
                    , i
                );
                auto _ind = IndentScoped();
                DumpStateNode(*stateNode);
            }
        }
    }
    template<>
    void DumpStateNodeDetails<AtomLayeringStateNode>(AtomLayeringStateNode& layeringNode)
    {
        ImGui::LogText(
            "%s BaseLayer:\n"
            , m_currentIndent.c_str()
        );
        {
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH_NODEREFERENCE(offsetof(AtomLayeringStateNode, BaseLayer));
            DumpLayeringInfo(layeringNode.BaseLayer);
        }
        ImGui::LogText(
            "%s Extra Layers: %d\n"
            , m_currentIndent.c_str()
            , layeringNode.layeringInfos.size
        );
        {
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH(offsetof(AtomLayeringStateNode, layeringInfos));
            for (uint16 i = 0; i < layeringNode.layeringInfos.size; i++)
            {
                AtomLayeringInfo& layerInfo = layeringNode.layeringInfos[i];
                ImGui::LogText(
                    "%s extra layer #%d.:\n"
                    , m_currentIndent.c_str()
                    , i
                );
                auto _ind = IndentScoped();
                IDOFFSETS_PUSH_NODEREFERENCE(i * sizeof(AtomLayeringInfo));
                DumpLayeringInfo(layerInfo);
            }
        }
    }
    void DumpStateNodeShort(AtomStateNode& stateNode)
    {
        ImGui::LogText(
            "%s%llX %s %s %s\n"
            , m_currentIndent.c_str()
            , &stateNode
            , stateNode.Unk028_GetTI().typeName
            , FormatTheCurrentOffsetStackForCPP().c_str()
            , FormatTheCurrentOffsetStackForReclassAndCheatEngine().c_str()
        );
    }
    void DumpStateNode(AtomStateNode& stateNode)
    {
        DumpStateNodeShort(stateNode);
        auto _ind = IndentScoped();
        ImGui::LogText(
            "%s Transitions: %d\n"
            , m_currentIndent.c_str()
            , stateNode.base8.Transitions.size
        );
        {
            auto _ind = IndentScoped();
            IDOFFSETS_PUSH(offsetof(AtomStateNode, base8.Transitions));
            for (uint16 i = 0; i < stateNode.base8.Transitions.size; i++)
            {
                IDOFFSETS_PUSH(sizeof(AtomStateTransitionTarget*) * i);
                AtomStateTransitionTarget* trTarget = stateNode.base8.Transitions[i];
                ImGui::LogText(
                    "%s transition #%d.:\n"
                    , m_currentIndent.c_str()
                    , i
                );
                auto _ind = IndentScoped();
                DumpStateTransitionTarget(*trTarget);
            }
        }
        DumpStateNodeCustom(stateNode);
    }
    void DumpStateNodeCustom(AtomStateNode& stateNode)
    {
        if (std::string_view(stateNode.Unk028_GetTI().typeName) == "AtomLayeringStateNode")
        {
            DumpStateNodeDetails(static_cast<AtomLayeringStateNode&>(stateNode));
        }
        else if (std::string_view(stateNode.Unk028_GetTI().typeName) == "AtomStateMachineNode")
        {
            DumpStateNodeDetails(static_cast<AtomStateMachineNode&>(stateNode));
        }
        else if (std::string_view(stateNode.Unk028_GetTI().typeName) == "AtomGraphStateNode")
        {
            DumpStateNodeDetails(static_cast<AtomGraphStateNode&>(stateNode));
        }
    }
    void DumpConditionExpression(AtomConditionExpression& condExpr)
    {
        ImGui::LogText(
            "%s referencedGraphVars: %d:\n"
            , m_currentIndent.c_str()
            , condExpr.referencedGraphVarsIndices.size
        );
        for (uint32 graphVarIdx : condExpr.referencedGraphVarsIndices)
        {
            auto _ind = IndentScoped();
            ImGui::LogText(
                "%s GraphVar: %d== %s\n"
                , m_currentIndent.c_str()
                , graphVarIdx
                , graphVariables_NewDemo_DEV[graphVarIdx]
            );
        }
        ImGui::LogText(
            "%s conditions: %d\n"
            , m_currentIndent.c_str()
            , condExpr.Conditions.size
        );
        if (condExpr.Conditions.size < 2) {
            return;
        }
        auto _ind = IndentScoped();
        uint16 topmostGroupSize = condExpr.Conditions[0].groupSizeIfDescribesConditionGroup;
        AtomCondition* first = &condExpr.Conditions[1];
        ImGui::LogText(m_currentIndent.c_str());
        DumpConditionChain(first, topmostGroupSize);
        ImGui::LogText("\n");
    }
    void SkipConditionsChain(AtomCondition*& condInOut)
    {
        AtomCondition* in = condInOut;
        condInOut++;
        for (size_t i = 0; i < in->groupSizeIfDescribesConditionGroup; i++)
        {
            SkipConditionsChain(condInOut);
        }
    }
    void DumpCondition_GraphVariable(AtomCondition* currentCond)
    {
        if (currentCond->word_A != 0xFFFF)
        {
            ImGui::LogText("ENTITYREFSMTH");
            return;
        }
        uint16 graphVarIdx = currentCond->ValueToTestReferenceID;
        const char* graphVarName = graphVariables_NewDemo_DEV[graphVarIdx];
        AtomDataContainerWrapper_DataType datatype = currentCond->ComparisonValue.DataType_0bool1float2int4xy5xyz6quat;
        const char* graphVarType = "UNKTYPE";
        switch (datatype)
        {
        case AtomDataContainerWrapper_DataType::Bool:
            graphVarType = "bool";
            break;
        case AtomDataContainerWrapper_DataType::Float:
            graphVarType = "float";
            break;
        case AtomDataContainerWrapper_DataType::Int:
            graphVarType = "int";
            break;
        case AtomDataContainerWrapper_DataType::XY:
            graphVarType = "Vec2";
            break;
        case AtomDataContainerWrapper_DataType::XYZ:
            graphVarType = "Vec3";
            break;
        case AtomDataContainerWrapper_DataType::XYZW:
            graphVarType = "Vec4";
            break;
        default:
            break;
        }
        const char* condOpText = "UNKCONDOP";
        auto condop = currentCond->ConditionalOperator;
        switch (condop)
        {
        case AtomCondition_ConditionalOperator::EQUALS:
            condOpText = "==";
            break;
        case AtomCondition_ConditionalOperator::GREATER_THAN:
            condOpText = ">";
            break;
        case AtomCondition_ConditionalOperator::GREATER_EQUAL:
            condOpText = ">=";
            break;
        case AtomCondition_ConditionalOperator::LESS_THAN:
            condOpText = "<";
            break;
        case AtomCondition_ConditionalOperator::LESS_EQUAL:
            condOpText = "<=";
            break;
        case AtomCondition_ConditionalOperator::NOT_EQUAL:
            condOpText = "!=";
            break;
        default:
            break;
        }
        static ImGuiTextBuffer valueToText;
        valueToText.clear();
        if (currentCond->isComparisonValueAnAnotherRTCPVarIndex)
        {
            uint16 rhsGraphVarIdx = (uint16&)currentCond->ComparisonValue.value;
            const char* rhsGraphVarName = graphVariables_NewDemo_DEV[rhsGraphVarIdx];
            valueToText.appendf("(%s)%s", graphVarType, rhsGraphVarName);
        }
        else
        {
            switch (datatype)
            {
            case AtomDataContainerWrapper_DataType::Bool:
                valueToText.appendf("%d", (bool&)currentCond->ComparisonValue.value);
                break;
            case AtomDataContainerWrapper_DataType::Float:
                valueToText.appendf("%f", (float&)currentCond->ComparisonValue.value);
                break;
            case AtomDataContainerWrapper_DataType::Int:
                valueToText.appendf("%d", (int&)currentCond->ComparisonValue.value);
                break;
            case AtomDataContainerWrapper_DataType::XY:
                valueToText.appendf("(%f, %f)"
                    , ((float*)&currentCond->ComparisonValue.value)[0]
                    , ((float*)&currentCond->ComparisonValue.value)[1]
                );
                break;
            case AtomDataContainerWrapper_DataType::XYZ:
                valueToText.appendf("(%f, %f, %f)"
                    , ((float*)&currentCond->ComparisonValue.value)[0]
                    , ((float*)&currentCond->ComparisonValue.value)[1]
                    , ((float*)&currentCond->ComparisonValue.value)[2]
                );
                break;
            case AtomDataContainerWrapper_DataType::XYZW:
                valueToText.appendf("(%f, %f, %f, %f)"
                    , ((float*)&currentCond->ComparisonValue.value)[0]
                    , ((float*)&currentCond->ComparisonValue.value)[1]
                    , ((float*)&currentCond->ComparisonValue.value)[2]
                    , ((float*)&currentCond->ComparisonValue.value)[3]
                );
                break;
            case AtomDataContainerWrapper_DataType::NoData_mb:
                break;
            default:
                break;
            }
        }
        ImGui::LogText(
            "((%s)%s %s %s)"
            , graphVarType
            , graphVarName
            , condOpText
            , valueToText.c_str()
        );
    }
    void DumpConditionChain(AtomCondition* firstInChain, uint16 chainSize)
    {
        ImGui::LogText("(");
        uint16 remainingChainSize = chainSize;
        AtomCondition* currentCond = firstInChain;
        for (size_t i = 0; i < remainingChainSize; i++)
        {
            switch (currentCond->ConditionType)
            {
            case AtomCondition_ConditionType::PLAYBACK_PERCENTAGE_mb:
                ImGui::LogText("PlaybackPercentage(%f)", (float&)currentCond->ComparisonValue.value);
                break;
            case AtomCondition_ConditionType::GRAPH_VARIABLE:
                DumpCondition_GraphVariable(currentCond);
                break;
            case AtomCondition_ConditionType::CONDITION_GROUP:
                DumpConditionChain(currentCond + 1, currentCond->groupSizeIfDescribesConditionGroup);
                break;
            case AtomCondition_ConditionType::UNK_1:
            case AtomCondition_ConditionType::UNK_4:
                ImGui::LogText("COND%d(%X, %d)", currentCond->ConditionType, currentCond->ValueToTestReferenceID, currentCond->groupSizeIfDescribesConditionGroup);
                break;
            default:
                ImGui::LogText("UNKCONDTYPE");
                break;
            }
            if (i != remainingChainSize - 1)
            {
                AtomCondition_ConjunctionOperator conjop = currentCond->ConjunctionOperator;
                const char* conjunctionOpText = "UNKCONJOP";
                switch (conjop)
                {
                case AtomCondition_ConjunctionOperator::AND:
                    conjunctionOpText = "AND";
                    break;
                case AtomCondition_ConjunctionOperator::OR:
                    conjunctionOpText = "OR";
                    break;
                default:
                    break;
                }
                ImGui::LogText(" %s ", conjunctionOpText);
            }
            SkipConditionsChain(currentCond);
            //uint16 subchainSizeToSkip = currentCond->groupSizeIfDescribesConditionGroup;
            //currentCond++;
            //for (size_t i = 0; i < subchainSizeToSkip; i++)
            //{
            //    SkipConditionsChain(currentCond);
            //}
        }
        ImGui::LogText(")");
    }
    void DumpAtomGraph(AtomGraph& atomGraph)
    {
        ImGui::LogToFile(-1, "AtomGraphDump.txt");
        {
            ImGui::LogText(
                "%s RootStateMachine:\n"
                , m_currentIndent.c_str()
            );
            {
                auto _ind = IndentScoped();
                IDOFFSETS_PUSH(offsetof(AtomGraph, RootStateMachine));
                DumpStateNode(*atomGraph.RootStateMachine);
            }
            ImGui::LogText(
                "%s custom transition system: %d:\n"
                , m_currentIndent.c_str()
                , atomGraph.AtomCustomTransitionSystem_->transitionCells.size
            );
            {
                auto _ind = IndentScoped();
                IDOFFSETS_PUSH(offsetof(AtomGraph, RootStateMachine));
                {
                    IDOFFSETS_PUSH(offsetof(AtomCustomTransitionSystem, transitionCells));
                    for (uint16 i = 0; i < atomGraph.AtomCustomTransitionSystem_->transitionCells.size; i++)
                    {
                        AtomTransitionCellDescription& trCell = atomGraph.AtomCustomTransitionSystem_->transitionCells[i];
                        ImGui::LogText(
                            "%s transition cell #%d.:\n"
                            , m_currentIndent.c_str()
                            , i
                        );
                        auto _ind = IndentScoped();
                        DumpStateNode(*trCell.stateMachineNode);
                    }
                }
            }
            ImGui::LogText(
                "%s root graph node:\n"
                , m_currentIndent.c_str()
            );
            {
                IDOFFSETS_PUSH(offsetof(AtomGraph, graphNode));
                auto _ind = IndentScoped();
                DumpStateNodeDetails(*atomGraph.graphNode);
            }
        }
        ImGui::LogFinish();
    }
private:
    bool IsNotSeenBefore(AtomGraphNode& graphNode)
    {
        auto [it, isNew] = m_seenBefore.insert(&graphNode);
        return isNew;
    }
    std::set<AtomGraphNode*> m_seenBefore;
};






#include "ACU/AtomAnimComponent.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
void DumpPlayerAnimgraph()
{
    AtomAnimComponent* animCpnt = ACU::GetPlayerCpnt_AtomAnimComponent();
    AtomGraph* graph = animCpnt->shared_AtomGraph_NewDemo_DEV->GetPtr();
    if (graph)
    {
        AtomGraphDumper dumper;
        dumper.DumpAtomGraph(*graph);
    }
}
#include "AtomGraphControls.h"
template<typename VarType>
void DrawGraphVariable(const char* label, GraphEvaluation& graphEvaluation, uint32 varnameHash, bool doNotifyIfSet = true);
template<>
void DrawGraphVariable<bool>(const char* label, GraphEvaluation& graphEvaluation, uint32 varnameHash, bool doNotifyIfSet)
{
    bool* v = GetGraphVariable<bool>(graphEvaluation, varnameHash);
    if (!v)
    {
        ImGui::Text("Variable `%s` not found in graph", label);
        return;
    }
    if (ImGui::Checkbox(label, v))
    {
        if (doNotifyIfSet)
            SetGraphVariable<bool>(graphEvaluation, varnameHash, *v);
    }
}
template<>
void DrawGraphVariable<int>(const char* label, GraphEvaluation& graphEvaluation, uint32 varnameHash, bool doNotifyIfSet)
{
    int* v = GetGraphVariable<int>(graphEvaluation, varnameHash);
    if (!v)
    {
        ImGui::Text("Variable `%s` not found in graph", label);
        return;
    }
    if (ImGui::InputInt(label, v))
    {
        if (doNotifyIfSet)
            SetGraphVariable<int>(graphEvaluation, varnameHash, *v);
    }
}
DEFINE_GAME_FUNCTION(FreeHashmap_mb, 0x14277A8A0, signed __int64, __fastcall, (uint64 hashmap, uint64 p_errorOut_mb));
void ClearBoneLayeringCache(AtomGraph& atomGraph)
{
    FreeHashmap_mb((uint64)&atomGraph.p98->hashmapOfSkeletonCombinations_mb, 0);
}
AtomGraph* CloneAtomGraph(AtomGraph& original)
{
    AtomGraph* clone = static_cast<AtomGraph*>(original.Unk018_Clone(0, 1));
    return clone;
}
void CloneAtomGraphExperiment()
{
    ACUSharedPtr_Strong<AtomGraph> shared_newdemodev(handle_PlayerAtomGraph);
    static AtomGraph* lastClone = nullptr;
    ImGuiTextBuffer buf;
    buf.appendf("%llx", lastClone);
    ImGui::Text("Last clone: %s", buf.c_str());
    if (ImGui::IsItemClicked())
    {
        ImGui::SetClipboardText(buf.c_str());
    }
    if (AtomGraph* original = shared_newdemodev.GetPtr())
    {
        if (ImGui::Button("Clone atomgraph"))
        {
            lastClone = CloneAtomGraph(*original);
        }
    }
}
#include "Hack_RemovableHood_Animations.h"
void DrawAtomGraphDumper()
{
    if (ImGui::Button("Dump Player's Animation graph"))
    {
        DumpPlayerAnimgraph();
    }
    AtomAnimComponent* animCpnt = ACU::GetPlayerCpnt_AtomAnimComponent();
    if (!animCpnt) { return; }
    GraphEvaluation* graphEvaluation = animCpnt->pD0;
    if (!graphEvaluation) { return; }
    ImGui::Separator();
    ImGui::Text("Current graph variables:");
    ImGui::SameLine();
    static bool notifyTheGraphWhenModifying = true;
    ImGui::Checkbox("Notify the graph when modifying", &notifyTheGraphWhenModifying);
    ImGui::Separator();
    // bool IsActing; // 0x1f156dfb/521498107
    DrawGraphVariable<bool>("IsActing", *graphEvaluation, 0x1f156dfb, notifyTheGraphWhenModifying);
    // bool ActingReInit; // 0xcaa1daa1/3399604897
    DrawGraphVariable<bool>("ActingReInit", *graphEvaluation, 0xcaa1daa1, notifyTheGraphWhenModifying);
    // bool ReflexShot; // 0xa9e7b83b/2850535483
    DrawGraphVariable<bool>("ReflexShot", *graphEvaluation, 0xa9e7b83b, notifyTheGraphWhenModifying);
    // bool ResetState; // 0x870d4aec/2265795308
    DrawGraphVariable<bool>("ResetState", *graphEvaluation, 0x870d4aec, notifyTheGraphWhenModifying);
    // bool IsPlayingSpecificAnimation; // 0xf9324be6/4180823014
    DrawGraphVariable<bool>("IsPlayingSpecificAnimation", *graphEvaluation, 0xf9324be6, notifyTheGraphWhenModifying);
    // bool IsBulkStation; // 0x2c2b039/46313529
    DrawGraphVariable<bool>("IsBulkStation", *graphEvaluation, 0x2c2b039, notifyTheGraphWhenModifying);
    // bool Prisoner; // 0x5df7a7ad/1576511405
    DrawGraphVariable<bool>("Prisoner", *graphEvaluation, 0x5df7a7ad, notifyTheGraphWhenModifying);
    // bool Investigate; // 0x52606110/1382048016
    DrawGraphVariable<bool>("Investigate", *graphEvaluation, 0x52606110, notifyTheGraphWhenModifying);
    // bool IsDrunk; // 0xde285267/3727184487
    DrawGraphVariable<bool>("IsDrunk", *graphEvaluation, 0xde285267, notifyTheGraphWhenModifying);
    // bool Juke; // 0x3c06ea45/1007086149
    DrawGraphVariable<bool>("Juke", *graphEvaluation, 0x3c06ea45, notifyTheGraphWhenModifying);
    // bool LatoucheWalk; // 0x44e2a880/1155704960
    DrawGraphVariable<bool>("LatoucheWalk", *graphEvaluation, 0x44e2a880, notifyTheGraphWhenModifying);
    // bool Pilotis; // 0x800e25a6/2148410790
    DrawGraphVariable<bool>("Pilotis", *graphEvaluation, 0x800e25a6, notifyTheGraphWhenModifying);
    // bool Sneak; // 0x8af229cb/2331126219
    DrawGraphVariable<bool>("Sneak", *graphEvaluation, 0x8af229cb, notifyTheGraphWhenModifying);
    // bool Child; // 0xe3727b2d/3815930669
    DrawGraphVariable<bool>("Child", *graphEvaluation, 0xe3727b2d, notifyTheGraphWhenModifying);
    // bool Hungover; // 0xad5e408f/2908635279
    DrawGraphVariable<bool>("Hungover", *graphEvaluation, 0xad5e408f, notifyTheGraphWhenModifying);
    // bool Hurt; // 0x67bfab24/1740614436
    DrawGraphVariable<bool>("Hurt", *graphEvaluation, 0x67bfab24, notifyTheGraphWhenModifying);
    // bool HighClass; // 0xcf930fb/217657595
    DrawGraphVariable<bool>("HighClass", *graphEvaluation, 0xcf930fb, notifyTheGraphWhenModifying);
    // bool ProudWalk; // 0x3c6ab8b1/1013627057
    DrawGraphVariable<bool>("ProudWalk", *graphEvaluation, 0x3c6ab8b1, notifyTheGraphWhenModifying);
    // int GeneralState; //0xdf85463d/3750053437
    // In Syndicate the values are:
    // 39 == dodge bullet; 32 == pick up body; 33 == kidnap; 36 == ragdoll; 29 == cut alarm bell; 28 == pick door lock; 27 == pick chest lock;
    // 23 == weird bird flight; 22 == aim pistol; 19 == shoved; 16 == massive stumble; 11 == milling arms freefall; 9 == swimming
    DrawGraphVariable<int>("GeneralState", *graphEvaluation, 0xdf85463d, notifyTheGraphWhenModifying);
    DrawGraphVariable<int>(g_rtcpDesc_HoodControlValue.varname, *graphEvaluation, g_rtcpDesc_HoodControlValue.varnameHash, notifyTheGraphWhenModifying);

    //if (ImGui::Button("Clear bone layering cache"))
    //{
    //    ClearBoneLayeringCache(*animCpnt->shared_AtomGraph_NewDemo_DEV->GetPtr());
    //}
    CloneAtomGraphExperiment();

}
