#ifndef gunTypes_H
#define gunTypes_H

struct Pistol {
	int currentAmmo;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;

	Pistol() {
		currentAmmo = 12;
		maxAmmo = 12;
		bulletSpeed = 800.f;
		bulletDamage = 25.f;
		fireRate = 0.2f;
	}
};

struct AssualtRifle {
	int currentAmmo;
	int maxAmmo;
	float bulletSpeed;
	float bulletDamage;
	float fireRate;

	AssualtRifle() {
		currentAmmo = 30;
		maxAmmo = 30;
		bulletSpeed = 1100.f;
		bulletDamage = 33.f;
		fireRate = 0.36f;
	}
};

#endif