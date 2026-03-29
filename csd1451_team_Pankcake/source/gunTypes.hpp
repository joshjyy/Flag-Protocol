/*!************************************************************************
  \file gunTypes.hpp
  \author
  Ye Min Thwin






  \brief
  This source file contains declarations for the gun types.

**************************************************************************/

#ifndef gunTypes_H
#define gunTypes_H

struct Pistol {
	const char* weaponName;
	int currentAmmo;
	int maxMagazine;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float reloadTimer;
	bool inReload;

	Pistol()
		: weaponName("Pistol"),
		currentAmmo(12),
		maxMagazine(120),
		maxAmmo(12),
		bulletSpeed(800.0f),
		bulletDamage(25.0f),
		fireRate(0.36f),
		reloadSpeed(2.0f),
		reloadTimer(0.0f),
		inReload(false)
	{
	}
};

struct AssaultRifle {
	const char* weaponName;
	int currentAmmo;
	int maxMagazine;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float reloadTimer;
	bool inReload;

	AssaultRifle() : 
		weaponName("Rifle"),
		currentAmmo(30),
		maxMagazine(300),
		maxAmmo(30),
		bulletSpeed(1100.f),
		bulletDamage(33.f),
		fireRate(0.1f),
		reloadSpeed(3.5f),
		reloadTimer(0.f),
		inReload(false)
	{}

};


struct Shotgun {
	const char* weaponName;
	int currentAmmo;
	int maxMagazine;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float reloadTimer;
	bool inReload;

	Shotgun() :
		weaponName("Shotgun"),
		currentAmmo(10),
		maxMagazine(30),
		maxAmmo(10),
		bulletSpeed(1000.0f),
		bulletDamage(50.0f),  // per pellet, 5 pellets = 75 total
		fireRate(0.8f),
		reloadSpeed(2.5f),
		reloadTimer(0.0f),
		inReload(false)
	{
	}
};

struct WeaponStats {
	const char* weaponName;
	int* currentAmmo;
	int* maxMagazine;
	int* maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float* reloadTimer;
	bool* inReload;
};

#endif