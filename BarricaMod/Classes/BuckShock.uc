//-----------------------------------------------------------
//
//-----------------------------------------------------------
class BuckShock extends UTProj_ShockBall;

simulated function ProcessTouch(Actor Other, vector HitLocation, vector HitNormal)
{
   //This doesn't look like I did anything, but I need to overwrite
   //super.ProcessTouch so that the orbs don't interact with each other
}