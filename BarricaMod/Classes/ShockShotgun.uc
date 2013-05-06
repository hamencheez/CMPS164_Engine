//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ShockShotgun extends UTWeap_ShockRifle;

     var int Orbs;

simulated function InstantFire()
{
	local int i;

            for(i=0; i < Orbs; i++)
            {
                super.InstantFire();
            }
}

simulated function Projectile ProjectileFire()
{
            local int i;
        local projectile P;

            for(i=0; i < Orbs; i++)
            {
                P = super.ProjectileFire();
            }

        return P;   }

DefaultProperties
{
 ShotCost(0) = 0
 ShotCost(1) = 0
 Spread(0) = 0.6
 Spread(1) = 0.6
 Orbs = 30;

	FireInterval(0)=+0.1 // Fire speed!
	FireInterval(1)=+0.1 // Fire speed!

 WeaponProjectiles(1) = class'BuckShock'
}