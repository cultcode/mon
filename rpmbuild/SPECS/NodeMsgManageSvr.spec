Name:		NodeMsgManageSvr
Version:	1.0
Release:	1%{?dist}
Summary:	Message Manage Service

Group:		Development/Tools
License:	Commercial
URL:		http://portal.titan.imgo.tv/
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

#BuildRequires:	
Requires:	SvrToolKit >= 1.1 liblog4c >= 1.0


%description


%prep
%setup -q


%build


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/home/Titan/%{name}
cp log4crc        %{buildroot}/home/Titan/%{name}/
cp %{name}        %{buildroot}/home/Titan/%{name}/
cp %{name}.config %{buildroot}/home/Titan/%{name}/


%post
echo "add cron job %{name}"
touch /var/spool/cron/root && \
crontab -l > ~/cron.bak && \
echo >> ~/cron.bak && \
sed -i -e '/%{name}/d' ~/cron.bak && \
sed -i '1i\* \* \* \* \* cd /home/Titan/%{name} && SvrScheduler.sh /home/Titan/%{name}/%{name} start > /dev/null  2>\&1' ~/cron.bak && \
sed -i -e '/^\s*$/d' ~/cron.bak && \
crontab ~/cron.bak
echo "%{name}-%{version} installed into /home/Titan/%{name}"


%postun
echo "remove cron job %{name}"
touch /var/spool/cron/root && \
crontab -l > ~/cron.bak && \
echo >> ~/cron.bak && \
sed -i -e '/%{name}/d' ~/cron.bak && \
sed -i -e '/^\s*$/d' ~/cron.bak && \
crontab ~/cron.bak
echo "%{name}-%{version} uninstalled from /home/Titan/%{name}"


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc
/home/Titan/%{name}/log4crc
/home/Titan/%{name}/%{name}
/home/Titan/%{name}/%{name}.config


%changelog

