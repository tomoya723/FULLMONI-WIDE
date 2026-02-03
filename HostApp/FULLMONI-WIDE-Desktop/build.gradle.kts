plugins {
    kotlin("jvm") version "2.0.0"
    application
}

group = "com.fullmoni"
version = "1.0.0"

repositories {
    mavenCentral()
}

dependencies {
    // USB Serial Communication
    implementation("com.fazecast:jSerialComm:2.10.4")
    
    // Coroutines
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.8.1")
    
    // Testing
    testImplementation(kotlin("test"))
}

application {
    mainClass.set("com.fullmoni.desktop.MainKt")
}

tasks.test {
    useJUnitPlatform()
}

kotlin {
    jvmToolchain(17)
}

// Fat JAR for easy distribution
tasks.register<Jar>("fatJar") {
    archiveClassifier.set("all")
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    manifest {
        attributes["Main-Class"] = "com.fullmoni.desktop.MainKt"
    }
    from(configurations.runtimeClasspath.get().map { if (it.isDirectory) it else zipTree(it) })
    with(tasks.jar.get())
}

// Configure run task for interactive stdin
tasks.named<JavaExec>("run") {
    standardInput = System.`in`
}
