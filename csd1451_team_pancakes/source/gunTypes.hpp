#ifndef gunTypes_H
#define gunTypes_H

struct Pistol {
	const char* weaponName;
	int currentAmmo;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float reloadTimer;
	bool inReload;

	Pistol() {
		weaponName = "Pistol";
		currentAmmo = 12;
		maxAmmo = 12;
		bulletSpeed = 800.f;
		bulletDamage = 25.f;
		fireRate = 0.36f;
		reloadSpeed = 2.f;
		reloadTimer = 0.f;
		inReload = false;
	}
};

struct AssaultRifle {
	const char* weaponName;
	int currentAmmo;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float reloadTimer;
	bool inReload;

	AssaultRifle() {
		weaponName = "Rifle";
		currentAmmo = 30;
		maxAmmo = 30;
		bulletSpeed = 1100.f;
		bulletDamage = 33.f;
		fireRate = 0.1f;
		reloadSpeed = 3.5f;
		reloadTimer = 0.f;
		inReload = false;
	}
};

struct WeaponStats {
	const char* weaponName;
	int* currentAmmo;
	int* maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
	float* reloadTimer;
	bool* inReload;
};

#endif