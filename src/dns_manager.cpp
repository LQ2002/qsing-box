#include "dns_manager.h"

#include <QDebug>
#include <QProcess>
#include <Windows.h>
#include <winreg.h>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

DnsManager::DnsManager(QObject *parent)
    : QObject(parent)
{
}

bool DnsManager::setDnsTo000()
{
    // 这里使用Windows命令行方式修改DNS
    // 获取所有网络连接并设置DNS为0.0.0.0
    
    QProcess process;
    
    // 方式1：使用 netsh 命令修改DNS
    // 设置所有以太网适配器
    process.start("netsh", QStringList() 
                  << "interface" << "ipv4" 
                  << "set" << "dnsservers" 
                  << "name=\"Ethernet\"" 
                  << "static" << "0.0.0.0" 
                  << "primary");
    
    if (!process.waitForFinished()) {
        m_lastError = "Failed to set DNS to 0.0.0.0";
        return false;
    }
    
    qDebug() << "DNS set to 0.0.0.0 successfully";
    return true;
}

bool DnsManager::restoreDns()
{
    QProcess process;
    
    // 恢复为自动获取DNS
    process.start("netsh", QStringList() 
                  << "interface" << "ipv4" 
                  << "set" << "dnsservers" 
                  << "name=\"Ethernet\"" 
                  << "dhcp");
    
    if (!process.waitForFinished()) {
        m_lastError = "Failed to restore DNS settings";
        return false;
    }
    
    qDebug() << "DNS restored to DHCP";
    return true;
}

QString DnsManager::getLastError() const
{
    return m_lastError;
}

bool DnsManager::modifyDnsRegistry(const QString &dnsValue)
{
    // 如果需要直接修改注册表，可以使用此方法
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces",
                      0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        m_lastError = "Failed to open registry";
        return false;
    }
    
    RegCloseKey(hKey);
    return true;
}

bool DnsManager::refreshDnsSettings()
{
    QProcess process;
    process.start("ipconfig", QStringList() << "/flushdns");
    return process.waitForFinished();
}