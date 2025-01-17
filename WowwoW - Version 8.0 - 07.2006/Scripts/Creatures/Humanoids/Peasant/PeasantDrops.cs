using System;
using Server.Items;
using System.Collections;
using Server;


///////////////////////////////////////////
namespace Server
{
	public class PeasantDrops
	{
		public static Loot[] HillsbradPeasant = new Loot[] {new Loot( typeof( BatteringHammer), 0.0222187f ) 
,new Loot( typeof( BattleKnife), 0.0428503f ) 
,new Loot( typeof( Battlesmasher), 0.0412633f ) 
,new Loot( typeof( BrightBelt), 0.0365021f ) 
,new Loot( typeof( BrightBracers), 0.0206316f ) 
,new Loot( typeof( BrightGloves), 0.031741f ) 
,new Loot( typeof( DecapitatingSword), 0.033328f ) 
,new Loot( typeof( DemonBand), 0.0238057f ) 
,new Loot( typeof( DwarvenMagestaff), 0.0253928f ) 
,new Loot( typeof( ForestLeatherBoots), 0.0523726f ) 
,new Loot( typeof( ForestLeatherBracers), 0.0365021f ) 
,new Loot( typeof( ForestLeatherGloves), 0.033328f ) 
,new Loot( typeof( ForestLeatherMantle), 0.0444374f ) 
,new Loot( typeof( FortifiedBelt), 0.0269798f ) 
,new Loot( typeof( FortifiedBoots), 0.031741f ) 
,new Loot( typeof( FortifiedChain), 0.0238057f ) 
,new Loot( typeof( FortifiedGauntlets), 0.0238057f ) 
,new Loot( typeof( FortifiedLeggings), 0.0412633f ) 
,new Loot( typeof( FortifiedSpaulders), 0.0238057f ) 
,new Loot( typeof( HeftyBattlehammer), 0.0253928f ) 
,new Loot( typeof( IvyclothBoots), 0.0253928f ) 
,new Loot( typeof( IvyclothBracelets), 0.0285669f ) 
,new Loot( typeof( IvyclothCloak), 0.0206316f ) 
,new Loot( typeof( JaggedStar), 0.0412633f ) 
,new Loot( typeof( LambentScaleCloak), 0.0253928f ) 
,new Loot( typeof( MassiveBattleAxe), 0.0476115f ) 
,new Loot( typeof( MossAgate), 0.095223f ) 
,new Loot( typeof( OrnateBronzeLockbox), 0.033328f ) 
,new Loot( typeof( PolishedZweihander), 0.0222187f ) 
,new Loot( typeof( RidgeCleaver), 0.031741f ) 
,new Loot( typeof( ScoutingTrousers), 0.0269798f ) 
,new Loot( typeof( ScoutingTunic), 0.0206316f ) 
,new Loot( typeof( Shadowgem), 0.0396762f ) 
,new Loot( typeof( ShimmeringTrousers), 0.0460244f ) 
,new Loot( typeof( SuperiorBelt), 0.0301539f ) 
,new Loot( typeof( SuperiorBracers), 0.0238057f ) 
,new Loot( typeof( SuperiorCloak), 0.0349151f ) 
,new Loot( typeof( DefenderSpaulders), 0.0476115f ) 
,new Loot( typeof( DwarvenMild), 4.55007f ) 
,new Loot( typeof( HealingPotion), 1.19822f ) 
,new Loot( typeof( HillsbradHumanSkull), 20.4555f ) 
,new Loot( typeof( LesserManaPotion), 0.674496f ) 
,new Loot( typeof( LinenCloth), 4.9024f ) 
,new Loot( typeof( MelonJuice), 2.1695f ) 
,new Loot( typeof( ScrollOfAgility), 0.211078f ) 
,new Loot( typeof( ScrollOfProtectionII), 0.187272f ) 
,new Loot( typeof( ScrollOfSpiritII), 0.163466f ) 
,new Loot( typeof( ScrollOfStrength), 0.260276f ) 
,new Loot( typeof( SilkCloth), 4.53896f ) 
,new Loot( typeof( WoolCloth), 23.117f ) 
,new Loot( typeof( BludgeoningCudgel), 0.206316f ) 
,new Loot( typeof( BoxShield), 0.144422f ) 
,new Loot( typeof( BrocadeBelt), 0.134899f ) 
,new Loot( typeof( BrocadeBracers), 0.111093f ) 
,new Loot( typeof( BrocadeCloak), 0.130138f ) 
,new Loot( typeof( BrocadeGloves), 0.153944f ) 
,new Loot( typeof( BrocadePants), 0.16664f ) 
,new Loot( typeof( BrocadeShoes), 0.117442f ) 
,new Loot( typeof( BrocadeShoulderpads), 0.160292f ) 
,new Loot( typeof( BrocadeVest), 0.155531f ) 
,new Loot( typeof( CedarWalkingStick), 0.238057f ) 
,new Loot( typeof( GougingPick), 0.238057f ) 
,new Loot( typeof( HuntingKnife), 0.201555f ) 
,new Loot( typeof( LinkedChainBelt), 0.0857007f ) 
,new Loot( typeof( LinkedChainBoots), 0.142834f ) 
,new Loot( typeof( LinkedChainBracers), 0.107919f ) 
,new Loot( typeof( LinkedChainCloak), 0.0904618f ) 
,new Loot( typeof( LinkedChainGloves), 0.095223f ) 
,new Loot( typeof( LinkedChainPants), 0.0920489f ) 
,new Loot( typeof( LinkedChainShoulderpads), 0.104745f ) 
,new Loot( typeof( LinkedChainVest), 0.0745913f ) 
,new Loot( typeof( MishandledRecurveBow), 0.230122f ) 
,new Loot( typeof( RawhideBelt), 0.141247f ) 
,new Loot( typeof( RawhideBoots), 0.142834f ) 
,new Loot( typeof( RawhideBracers), 0.122203f ) 
,new Loot( typeof( RawhideCloak), 0.142834f ) 
,new Loot( typeof( RawhideGloves), 0.182511f ) 
,new Loot( typeof( RawhidePants), 0.157118f ) 
,new Loot( typeof( RawhideShoulderpads), 0.12379f ) 
,new Loot( typeof( RawhideTunic), 0.0920489f ) 
,new Loot( typeof( ShinyWarAxe), 0.249167f ) 
,new Loot( typeof( ShoddyBlunderbuss), 0.207904f ) 
,new Loot( typeof( SmallRoundShield), 0.158705f ) 
,new Loot( typeof( StandardClaymore), 0.195207f ) 
,new Loot( typeof( WarpedBlade), 0.234883f ) 
,new Loot( typeof( WoodenMaul), 0.222187f ) 
};

		public static Loot[] ShadowforgePeasant = new Loot[] {new Loot( typeof( AbjurersCrystal), 0.0213451f ) 
,new Loot( typeof( AlabasterPlateGauntlets), 0.0262709f ) 
,new Loot( typeof( AlabasterPlateHelmet), 0.0213451f ) 
,new Loot( typeof( Aquamarine), 0.126428f ) 
,new Loot( typeof( BlackDiamond), 0.573033f ) 
,new Loot( typeof( CouncillorsSash), 0.0213451f ) 
,new Loot( typeof( CrusadersPauldrons), 0.022987f ) 
,new Loot( typeof( DivinerLongStaff), 0.0279128f ) 
,new Loot( typeof( ImperialRedMantle), 0.0262709f ) 
,new Loot( typeof( LibramOfConstitution), 0.022987f ) 
,new Loot( typeof( LordsCrest), 0.0328386f ) 
,new Loot( typeof( MarbleCircle), 0.0246289f ) 
,new Loot( typeof( MarbleNecklace), 0.0246289f ) 
,new Loot( typeof( MithrilLockbox), 0.169119f ) 
,new Loot( typeof( SerpentskinLeggings), 0.022987f ) 
,new Loot( typeof( SmashingStar), 0.0344805f ) 
,new Loot( typeof( StarRuby), 0.159267f ) 
,new Loot( typeof( ThaumaturgistStaff), 0.0246289f ) 
,new Loot( typeof( WanderersCloak), 0.0311966f ) 
,new Loot( typeof( WizardsHand), 0.0328386f ) 
,new Loot( typeof( ACrumpledUpNote), 0.118219f ) 
,new Loot( typeof( DarkIronFannyPack), 7.14239f ) 
,new Loot( typeof( DarkIronResidue), 13.1354f ) 
,new Loot( typeof( HomemadeCherryPie), 5.56449f ) 
,new Loot( typeof( MajorHealingPotion), 1.21174f ) 
,new Loot( typeof( MorningGloryDew), 2.55156f ) 
,new Loot( typeof( Runecloth), 21.2482f ) 
,new Loot( typeof( ScrollOfAgilityIII), 0.26435f ) 
,new Loot( typeof( ScrollOfProtectionIV), 0.164193f ) 
,new Loot( typeof( ScrollOfSpiritIV), 0.164193f ) 
,new Loot( typeof( ScrollOfStrengthIII), 0.27256f ) 
,new Loot( typeof( SmallSackOfCoins), 0.022987f ) 
,new Loot( typeof( SuperiorManaPotion), 0.737226f ) 
,new Loot( typeof( BalancedWarAxe), 0.0903061f ) 
,new Loot( typeof( BulkyMaul), 0.100158f ) 
,new Loot( typeof( CloutMace), 0.108367f ) 
,new Loot( typeof( CrestedBuckler), 0.0591094f ) 
,new Loot( typeof( DeflectingTower), 0.0525417f ) 
,new Loot( typeof( FineLongsword), 0.131354f ) 
,new Loot( typeof( JaggedAxe), 0.0837383f ) 
,new Loot( typeof( LaminatedScaleArmor), 0.0443321f ) 
,new Loot( typeof( LaminatedScaleBelt), 0.0476159f ) 
,new Loot( typeof( LaminatedScaleBoots), 0.0640352f ) 
,new Loot( typeof( LaminatedScaleBracers), 0.0328386f ) 
,new Loot( typeof( LaminatedScaleCirclet), 0.0508998f ) 
,new Loot( typeof( LaminatedScaleCloak), 0.0410482f ) 
,new Loot( typeof( LaminatedScaleGloves), 0.0492578f ) 
,new Loot( typeof( LaminatedScalePants), 0.0426901f ) 
,new Loot( typeof( LaminatedScaleShoulderpads), 0.0426901f ) 
,new Loot( typeof( LightPlateBelt), 0.022987f ) 
,new Loot( typeof( LightPlateBoots), 0.0426901f ) 
,new Loot( typeof( LightPlateBracers), 0.0279128f ) 
,new Loot( typeof( LightPlateChestpiece), 0.0213451f ) 
,new Loot( typeof( LightPlatePants), 0.0279128f ) 
,new Loot( typeof( LightPlateShoulderpads), 0.0328386f ) 
,new Loot( typeof( PrimedMusket), 0.114935f ) 
,new Loot( typeof( RecurveLongBow), 0.123145f ) 
,new Loot( typeof( ShinyBracelet), 0.0213451f ) 
,new Loot( typeof( SmoothCloak), 0.0541836f ) 
,new Loot( typeof( SmoothLeatherArmor), 0.0591094f ) 
,new Loot( typeof( SmoothLeatherBelt), 0.045974f ) 
,new Loot( typeof( SmoothLeatherBoots), 0.0492578f ) 
,new Loot( typeof( SmoothLeatherBracers), 0.0591094f ) 
,new Loot( typeof( SmoothLeatherGloves), 0.0607513f ) 
,new Loot( typeof( SmoothLeatherHelmet), 0.0558256f ) 
,new Loot( typeof( SmoothLeatherPants), 0.0508998f ) 
,new Loot( typeof( SmoothLeatherShoulderpads), 0.0344805f ) 
,new Loot( typeof( SpikedDagger), 0.0673191f ) 
,new Loot( typeof( StoutWarStaff), 0.0985157f ) 
,new Loot( typeof( TaperedGreatsword), 0.0640352f ) 
,new Loot( typeof( TwillBelt), 0.0558256f ) 
,new Loot( typeof( TwillBoots), 0.0558256f ) 
,new Loot( typeof( TwillBracers), 0.0722448f ) 
,new Loot( typeof( TwillCloak), 0.0410482f ) 
,new Loot( typeof( TwillCover), 0.0394063f ) 
,new Loot( typeof( TwillGloves), 0.0623933f ) 
,new Loot( typeof( TwillPants), 0.0607513f ) 
,new Loot( typeof( TwillShoulderpads), 0.0558256f ) 
,new Loot( typeof( TwillVest), 0.0673191f ) 
};

		public static Loot[] NorthshirePeasant = new Loot[] {new Loot( typeof( LinenCloth), 26.087f ) 
};

		public static Loot[] EastvalePeasant = new Loot[] {new Loot( typeof( LinenCloth), 26.087f ) 
};

	}
}