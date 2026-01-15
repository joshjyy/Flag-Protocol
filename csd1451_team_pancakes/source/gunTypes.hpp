#ifndef gunTypes_H
#define gunTypes_H

struct Pistol {
	int currentAmmo;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;

	Pistol() {
		currentAmmo = 12;
		maxAmmo = 12;
		bulletSpeed = 800.f;
		bulletDamage = 25.f;
		fireRate = 0.36f;
		reloadSpeed = 2.f;
	}
};

struct AssaultRifle {
	int currentAmmo;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;

	AssaultRifle() {
		currentAmmo = 30;
		maxAmmo = 30;
		bulletSpeed = 1100.f;
		bulletDamage = 33.f;
		fireRate = 0.1f;
		reloadSpeed = 3.5f;
	}
};

struct WeaponStats {
	int* currentAmmo;
	int* maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;
	float reloadSpeed;
};

#endif